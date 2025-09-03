#include "HttpRequest.hpp"

bool HttpRequest::pickServerConfig() {
    //Pick correct server config
    //Set it at _server

    std::cout << "Looking for port (" << _serverInfo.first << ") and IP (" << _serverInfo.second << ")" << std::endl;
    const std::string host = _headers.find("host")->second;
    size_t pos = host.find(':', 0);
    std::string searchedName = host.substr(0, pos);

    std::cout << searchedName << "\n";

    const std::vector< ServerConfig > &serverList = _config.getServerConfig();
    std::vector< ServerConfig >::const_iterator itServer = serverList.begin();

    for (; itServer != serverList.end() ; ++itServer)   // iterate through server list
    {
        const std::vector< std::string > &configServerNames = itServer->getServerName();
        std::vector< std::string >::const_iterator itName = configServerNames.begin();
        for (; itName != configServerNames.end() ; ++itName)    // iterate through server names
        {
            const std::vector< int > &configPort = itServer->getListenPort();
            std::vector< int >::const_iterator itPort = configPort.begin();
            for (; itPort != configPort.end() ; ++itPort)    // iterate through port
            {
                std::cout << *itName << "   |   " << *itPort << std::endl;
            //   if (*itName == searchedName
                    // && _serverInfo.first == *itPort
                    // && _serverInfo.second == )
            }

        }

    }


    const std::vector< ServerConfig > &config = _config.getServerConfig();
    _server = &(*config.begin());

    return false;
}