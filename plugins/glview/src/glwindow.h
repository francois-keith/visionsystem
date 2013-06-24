/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details. */


#ifndef GLX_WINDOW_H
#define GLX_WINDOW_H

#include <string>
#include <vector>

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/extensions/xf86vmode.h>
#include <vision/image/image.h>

#include <stdint.h>

using namespace std ;
using namespace vision;

class GLWindow
{

    public:

        GLWindow ( string title, int width, int height, Bool fullscreen );
        ~GLWindow() ;

        void draw( Image<unsigned char, MONO>* img ) ;
        void draw( Image<uint32_t, RGB>* img ) ;
        void draw_caption(int captionPos, vector<string> s);
        void swap_buffers() ;
        int  next_event ( XEvent *event ) ;
        void processEvents( XEvent event ) ;

        void print ( const char* s ) ;        // OpenGL stuff to draw a text

    private:

        Display* dpy_;                // window stuff
        int screen_;
        Window win_;
        GLXContext ctx_;
        XSetWindowAttributes attr_;

        Bool fullScreen_;
        Bool doubleBuffered_;

        XF86VidModeModeInfo deskMode_;
        unsigned int depth_;

        int winWidth_, winHeight_;        // window size

        char    font_[255];            // font used for info display
        GLuint     fontBase_;            // liste des fontes

        void resize();

        Image< uint32_t, RGB >        *RGB_img  ;
} ;


/* attributes for a single buffered visual in RGBA format with at least
 * 4 bits per color and a 16 bit depth buffer */

static int attrListSgl[] = { GLX_RGBA, GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 16,
    None
};

/* attributes for a double buffered visual in RGBA format with at least
 4 bits per color and a 16 bit depth buffer */

static int attrListDbl[] = { GLX_RGBA, GLX_DOUBLEBUFFER,
    GLX_RED_SIZE, 4,
    GLX_GREEN_SIZE, 4,
    GLX_BLUE_SIZE, 4,
    GLX_DEPTH_SIZE, 16,
    None
};


// --- DOC --------------------------------------------------------

/*!
* \file glwindow.h
* \brief Declaration of class GLWindow.
* \author Sebastien Druon druon@lirmm.fr, Pierre Gergondet pierre.gergondet@gmail.com, Gregoire Milliez gregmilliez@hotmail.com
* \version 0.0.0
*
* Declaration of the GLWindow class.
*
*/

/*!
* \class GLWindow glwindow.h "Definition"
* \brief Declaration of the GLwindow class.
* \param dpy_ is a buffer to specifies the connection to the X server.
* \param screen_  is the default screen number referenced in the XOpenDisplay routine for dpy_.
* \param win_ is a buffer of what we are currently displaying.
* \param ctx_ is the GLX context.
* \param attr_ is the window attribute.
* \param fullScreen_ is a bool to know if the window is fullScreen.
* \param doubleBuffered_ is a bool and is true if there are 2 buffers, use as a test in swap_buffer().
* \param deskMode_ is a XF86VidModeModeInfo.
* \param depth_ is the scene depth.
* \param winWidth_ is the window's width.
* \param winHeight_ is the window's height.
* \param imWidth_ is the image's width.
* \param imHeight_ is the image's height
* \param font_ is glx font.
* \param fontBase_ is glx font base.
* \param attrListSgl[] is a static attribute for a single buffered visual in RGBA format with at least
* 4 bits per color and a 16 bit depth buffer.
* \param attrListDbl[] attributes for a double buffered visual in RGBA format with at least
 4 bits per color and a 16 bit depth buffer.
*/

/*!
* \fn GLWindow::GLWindow( string title, int width, int height, Bool fullscreen )
* \brief constructor of GLWindow.
* \param title is the window's title.
* \param width is the window's width.
* \param height is the window's height.
* \param fullscreen is usefull to know if the window should be display fullscreen or not.
*/

/*!
* \fn GLWindow::~GLWindow()
* \brief destructor of GLWindow.
* \note destroy the glx context and switch back to original desktop resolution if we were in fullScreen.
*/

/*!
* \fn void GLWindow::draw( Image<unsigned char>* img )
* \brief This is a method to draw an image in the window background using glDrawPixels.
* \param img is an image in usigned char format.
*/

/*!
* \fn void GLWindow::swap_buffers()
* \brief This is a method to swap dpy_ and win_ buffers, that is to say the current display and the new display.
*/

/*!
* \fn void GLWindow::processEvents( Image<unsigned char>* img, bool* end, vector<GLplugin*> callbacks)
* \brief This is a method to analyse the event and call callbacks functions.
* \param img is the image we want to display.
* \param end is a bool to know if evrything is finish.
* \param callbacks is the callbacks vector.
*/

/*!
* \fn void draw_caption(int captionPos, vector<string> s)
* \brief This is a method to make a grey quad to display the cation string s
* \param captionPos is the Position of the first caption in the string vector s. I.e. older printed caption.
* \param s is the string we want to print.
* \note this method calls print.
*/

/*!
* \fn void GLWindow::print( const char* s)
* \brief This is a method which use openGL function to display string.
* \param s is the string we want to display.
* \note this method is called from draw_caption.
*/

#endif
