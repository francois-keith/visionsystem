#include "xmlrpc-client.h"

namespace visionsystem
{

XMLRPCClient::XMLRPCClient( VisionSystem * vs, std::string sandbox )
: Plugin(vs, "xmlrpc-client", sandbox),
  m_client(0),
  m_server_name("127.0.0.1"), m_server_port(1234)
{}

XMLRPCClient::~XMLRPCClient()
{
    delete m_client;
}

bool XMLRPCClient::pre_fct()
{
    std::string filename = get_sandbox() + std::string("/xmlrpc-client.conf") ;

    try
    {
        read_config_file ( filename.c_str() );
    }
    catch( ... )
    {
        /* unexisting config file, assume default value */
    }

    m_client = new XmlRpcClient(m_server_name.c_str(), m_server_port);

    whiteboard_write<XMLRPCClient *> ( "plugin_xmlrpc-client", this );

    return true;
}

void XMLRPCClient::preloop_fct()
{}

void XMLRPCClient::loop_fct()
{}

bool XMLRPCClient::post_fct()
{
    m_client->close();

    return true;
}

bool XMLRPCClient::execute (const char *method, XmlRpcValue const &params, XmlRpcValue &result)
{
    return m_client->execute(method, params, result);
}

void XMLRPCClient::parse_config_line( std::vector<std::string> & line )
{
    if( fill_member(line, "Host", m_server_name) ) return;

    if( fill_member(line, "Port", m_server_port) ) return;
}

} // namespace visionsystem

PLUGIN(visionsystem::XMLRPCClient)

