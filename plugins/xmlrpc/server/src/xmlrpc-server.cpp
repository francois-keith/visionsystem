#include "xmlrpc-server.h"

namespace visionsystem
{

XMLRPCServer::XMLRPCServer( VisionSystem * vs, std::string sandbox )
: Plugin(vs, "xmlrpc-server", sandbox),
  m_server_port(1234), m_close(false), m_server_th(0)
{}
    
XMLRPCServer::~XMLRPCServer()
{
    delete m_server_th;
}

void XMLRPCServer::WorkThread()
{
    while(!m_close)
    {
        m_server.work(1);
    }
    m_server.shutdown();
}

bool XMLRPCServer::pre_fct()
{
    std::string filename = get_sandbox() + std::string("/xmlrpc-server.conf") ;
    
    try
    {
        read_config_file ( filename.c_str() );
    }
    catch( ... )
    {
        /* unexisting config file, assume default value */
    }

    m_server.bindAndListen(m_server_port);
    m_server_th = new boost::thread(boost::bind(&XMLRPCServer::WorkThread, this));    

    whiteboard_write<XMLRPCServer *> ( "plugin_xmlrpc-server", this );

    return true;
}

void XMLRPCServer::preloop_fct()
{}

void XMLRPCServer::loop_fct()
{}

bool XMLRPCServer::post_fct()
{
    m_close = true;
    m_server_th->join();

    return true;
}

void XMLRPCServer::AddMethod(XmlRpcServerMethod * method)
{
    m_server.addMethod(method);
}

void XMLRPCServer::parse_config_line( std::vector<std::string> & line )
{
    fill_member(line, "Port", m_server_port);
} 

} // namespace visionsystem

PLUGIN(visionsystem::XMLRPCServer)

