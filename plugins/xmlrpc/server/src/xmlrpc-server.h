#ifndef _H_XMLRPCSERVER_H_
#define _H_XMLRPCSERVER_H_

#include <configparser/configparser.h>
#include <visionsystem/plugin.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

/* Library available at http://xmlrpcpp.sourceforge.net/ */
#include "XmlRpc.h"

using namespace XmlRpc;

namespace visionsystem
{

class XMLRPCServer : public visionsystem::Plugin, public configparser::WithConfigFile
{
public:
    XMLRPCServer( visionsystem::VisionSystem * vs, std::string sandbox );
    ~XMLRPCServer();

    /* Implements method from Plugin */
    bool pre_fct();
    void preloop_fct();
    void loop_fct();
    bool post_fct();

    /* Used to register new method to the server */
    void AddMethod(XmlRpcServerMethod * method);

private:
    /* Configparser method */
    void parse_config_line( std::vector<std::string> & line );

    void WorkThread();

private:
    XmlRpcServer m_server;
    short m_server_port;
    bool m_close;
    boost::thread * m_server_th;
};

} // namespace visionsystem

#endif
