#ifndef _H_XMLRPCCLIENT_H_
#define _H_XMLRPCCLIENT_H_

#include <configparser/configparser.h>
#include <visionsystem/plugin.h>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

/* Library available at http://xmlrpcpp.sourceforge.net/ */
#include "XmlRpc.h"

using namespace XmlRpc;

namespace visionsystem
{

class XMLRPCClient : public visionsystem::Plugin, public configparser::WithConfigFile
{
public:
    XMLRPCClient( visionsystem::VisionSystem * vs, std::string sandbox );
    ~XMLRPCClient();

    /* Implements method from Plugin */
    bool pre_fct();
    void preloop_fct();
    void loop_fct();
    bool post_fct();

    /* Used to access methods on server */
    bool execute (const char *method, XmlRpcValue const &params, XmlRpcValue &result);

private:
    /* Configparser method */
    void parse_config_line( std::vector<std::string> & line );

private:
    XmlRpcClient * m_client;
    std::string m_server_name;
    short m_server_port;
};

} // namespace visionsystem

#endif
