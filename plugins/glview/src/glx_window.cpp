/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */



#include <iostream>
#include <stdio.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/keysym.h>

#include "glwindow.h"

#define LINES 		28
#define LINE_HEIGHT	17
#define LINE_OFFSET	12

using namespace std;

GLWindow::GLWindow(string title, int width, int height,	Bool fullscreen) {

	XVisualInfo *vi;
	Colormap cmap;
	int dpyWidth, dpyHeight;
	int i;
	int glxMajorVersion, glxMinorVersion;
	int vidModeMajorVersion, vidModeMinorVersion;
	XF86VidModeModeInfo **modes;
	int modeNum;
	int bestMode;
	Atom wmDelete;
	Window winDummy;
	unsigned int borderDummy;

	fullScreen_ = fullscreen;

	RGB_img = NULL ;
	RGB_img = NULL ;

	/* set best mode to current */
	bestMode = 0;

	/* get a connection */
	dpy_ = XOpenDisplay(0);
	screen_ = DefaultScreen(dpy_);
	XF86VidModeQueryVersion(dpy_, &vidModeMajorVersion, &vidModeMinorVersion);
	cout<< "[glview] XF86VidModeExtension-Version " << vidModeMajorVersion << "." <<	vidModeMinorVersion << endl ;
	XF86VidModeGetAllModeLines(dpy_, screen_, &modeNum, &modes);

	/* save desktop-resolution before switching modes */
	deskMode_ = *modes[0];

	/* look for mode with requested resolution */
	for (i = 0; i < modeNum; i++) {
		if ((modes[i]->hdisplay == width) && (modes[i]->vdisplay == height)) {
			bestMode = i;
		}
	}

	/* get an appropriate visual */
	vi = glXChooseVisual(dpy_, screen_, attrListDbl);
	if (vi == NULL) {
		vi = glXChooseVisual(dpy_, screen_, attrListSgl);
		doubleBuffered_ = False;
		cout << "[glview] Only Singlebuffered Visual!" << endl ;
	} else {
		doubleBuffered_ = True;
		cout << "[glview] Got doubleBuffered_ Visual!" << endl ;
	}
	glXQueryVersion(dpy_, &glxMajorVersion, &glxMinorVersion);
	cout << "[glview] glX-Version " << glxMajorVersion << "." << glxMinorVersion << endl ;

	/* create a GLX context */
	ctx_ = glXCreateContext(dpy_, vi, 0, GL_TRUE);

	/* create a color map */
	cmap = XCreateColormap(dpy_, RootWindow(dpy_, vi->screen), vi->visual,
			AllocNone);
	attr_.colormap = cmap;
	attr_.border_pixel = 0;
	if (fullScreen_) {
		XF86VidModeSwitchToMode(dpy_, screen_, modes[bestMode]);
		XF86VidModeSetViewPort(dpy_, screen_, 0, 0);
		dpyWidth = modes[bestMode]->hdisplay;
		dpyHeight = modes[bestMode]->vdisplay;
		cout << "[glview] Resolution " << dpyWidth << "x" << dpyHeight ;
		XFree(modes);

		/* create a fullscreen window */
		attr_.override_redirect = True;
		attr_.event_mask = ExposureMask | KeyPressMask | ButtonPressMask
				| StructureNotifyMask;

		winWidth_ = dpyWidth;
		winHeight_ = dpyHeight;

		win_ = XCreateWindow(dpy_, RootWindow(dpy_, vi->screen), 0, 0,
				dpyWidth, dpyHeight, 0, vi->depth, InputOutput, vi->visual,
				CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect,
				&attr_);

		XWarpPointer(dpy_, None, win_, 0, 0, 0, 0, 0, 0);
		XMapRaised(dpy_, win_);
		XGrabKeyboard(dpy_, win_, True, GrabModeAsync, GrabModeAsync,
				CurrentTime);
		XGrabPointer(dpy_, win_, True, ButtonPressMask, GrabModeAsync,
				GrabModeAsync, win_, None, CurrentTime);
	} else {
		/* create a window in window mode */
		attr_.event_mask = ExposureMask | KeyPressMask | ButtonPressMask
				| StructureNotifyMask;
		winWidth_ = width;
		winHeight_ = height;
		win_ = XCreateWindow(dpy_, RootWindow(dpy_, vi->screen), 0, 0, width,
				height, 0, vi->depth, InputOutput, vi->visual, CWBorderPixel
						| CWColormap | CWEventMask, &attr_);
		/* only set window title and handle wm_delete_events if in windowed mode */
		wmDelete = XInternAtom(dpy_, "WM_DELETE_WINDOW", True);
		XSetWMProtocols(dpy_, win_, &wmDelete, 1);
		XSetStandardProperties(dpy_, win_, title.c_str(), title.c_str(), None,
				NULL, 0, NULL);
		XMapRaised(dpy_, win_);
	}
	/* connect the glx-context to the window */

	glXMakeCurrent(dpy_, win_, ctx_);

	int x, y;
	XGetGeometry(dpy_, win_, &winDummy, &x, &y, (unsigned int *) &width,
			(unsigned int *) &height, &borderDummy, &depth_);

	cout << "[glview] Depth " << depth_ << endl ;

	if (glXIsDirect(dpy_, ctx_))
		cout << "[glview] Congrats, you have Direct Rendering!" << endl ;
	else
		cout << "[glview] Sorry, no Direct Rendering possible!" << endl ;

	/* Init fonts */

	sprintf(font_, "fixed");

	fontBase_ = glGenLists(256);

	if (!glIsList(fontBase_)) {
		cerr << "[glview] ERROR : Out of display lists. - Exiting." << endl ;
		exit(0);
	}

	XFontStruct* font_info;
	int first, last;

	font_info = XLoadQueryFont(dpy_, font_);

	if (!font_info) {
		cerr << "[glview] ERROR: XLoadQueryFont() failed - Exiting." << endl ;
		exit(0);
	}

	first = font_info->min_char_or_byte2;
	last = font_info->max_char_or_byte2;

	glXUseXFont(font_info->fid, first, last - first + 1, fontBase_ + first);

}

GLWindow::~GLWindow() {
	if (ctx_) {
		if (!glXMakeCurrent(dpy_, None, NULL)) {
			cerr << "[glview] ERROR: Could not release drawing context." << endl ;
		}
		glXDestroyContext(dpy_, ctx_);
		ctx_ = NULL;
	}
	/* switch back to original desktop resolution if we were in fullScreen */
	if (fullScreen_) {
		XF86VidModeSwitchToMode(dpy_, screen_, &deskMode_);
		XF86VidModeSetViewPort(dpy_, screen_, 0, 0);
	}
	XCloseDisplay ( dpy_);

}

XEvent GLWindow::processEvents() {

	XEvent event;

	/* handle the events in the queue */
	while (XPending(dpy_) > 0) {
		XNextEvent(dpy_, &event);
		switch (event.type) {

		case Expose:
			if (event.xexpose.count != 0)
			break;
				draw(RGB_img) ;
			break;

		case ConfigureNotify:

			/* call resize only if our window-size changed */
			if ((event.xconfigure.width != winWidth_)
					|| (event.xconfigure.height != winHeight_)) {

				winWidth_ = event.xconfigure.width;
				winHeight_ = event.xconfigure.height;

			}
			break;

		case ClientMessage:
			if (*XGetAtomName(dpy_, event.xclient.message_type)
					== *"WM_PROTOCOLS") {
				cout << "[glview] Exiting sanely..." << endl ;
			}
			break;

		default:
			break;
		}

	}

	return event ;
}

void GLWindow::draw( Image<uint32_t,RGB> *img) {

	RGB_img = img ;

	unsigned int imWidth = img->width ;
	unsigned int imHeight = img->height ;

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	if (winHeight_ == 0) /* Prevent A Divide By Zero If The Window Is Too Small */
		winHeight_ = 1;

	glViewport(0, 0, winWidth_, winHeight_);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-0.5, (double) imWidth - 0.5, (double) imHeight - 0.5, -0.5,
			-1.0, 1.0);

	glPixelZoom( (double) winWidth_ / (double) imWidth, 
	            -(double) winHeight_ / (double) imHeight ) ;

	glClear(GL_COLOR_BUFFER_BIT);

	glFlush();

	// Draw the camera picture

	glRasterPos2d(-0.5, -0.5);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, imWidth * sizeof(unsigned char ));

	glDrawPixels( imWidth, imHeight, GL_RGBA, GL_UNSIGNED_BYTE, img->raw_data );		

	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

	glClear(GL_DEPTH_BUFFER_BIT);

}


void GLWindow::print(const char* s) {
	if (!glIsList(fontBase_)) {
		cerr << "[glview] Bad display list. - Exiting." << endl ;
		exit(-1);
	} else if (s && strlen(s)) {
		glPushAttrib(GL_LIST_BIT);
		glListBase( fontBase_);
		glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte *) s);
		glPopAttrib();
	}
}

void GLWindow::draw_caption(int captionPos, vector<string> s) {

	unsigned int start=0;

	if (s.size() == 0)
		return;

	// Draw a grey background box for the text

	glColor4f(0.0f, 0.0f, 0.0f, 0.4f);

	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int nTopOfBox = RGB_img->width - LINES * LINE_HEIGHT;
	int nBotOfBox = RGB_img->height + (s.size() -LINES) * LINE_HEIGHT;

	glBegin(GL_QUADS);
	glVertex2d(0.0, nTopOfBox);
	glVertex2d( RGB_img->width, nTopOfBox);
	glVertex2d( RGB_img->height, nBotOfBox);
	glVertex2d(0.0, nBotOfBox);
	glEnd();

	// Draw the caption text in yellow

	glColor3f(1, 1, 0);

	// Displays the LINES last lines

	if (s.size() > LINES)
		start = s.size() - LINES - captionPos;
	else
		start = 0;

	for (size_t i = start; i < s.size() && i <= (LINES + start); i++) {
		glRasterPos2d( RGB_img->width/ 50.0, LINE_OFFSET + RGB_img->height - (LINES - i +start)
				* LINE_HEIGHT);
		print(s[i].c_str());
	}

}

void GLWindow::swap_buffers() {

	// Swap buffers if needed


	if (doubleBuffered_) {
		glXSwapBuffers(dpy_, win_);
	}

}

