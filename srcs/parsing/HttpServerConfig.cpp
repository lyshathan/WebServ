#include "HttpRequest.hpp"

bool HttpRequest::pickServerConfig() {
    //Pick correct server config
    //Set it at _server

    
    std::map<std::string, std::string>::iterator it = _headers.begin();

    for (; it != _headers.end(); ++it) {
        std::cout << it->first << " : " << it->second << "\n";
    }
    const std::vector< ServerConfig > config = _config.getServerConfig();
    _server = &(*config.begin());

    return false;
}