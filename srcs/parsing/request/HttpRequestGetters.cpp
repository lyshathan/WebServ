#include "HttpRequest.hpp"

/******************************************************************************/
/*							PARSE  GETTERS									  */
/******************************************************************************/

const std::string& HttpRequest::getMethod()const {return _method;}

const std::string& HttpRequest::getUri() const {return _uri;}

const std::string& HttpRequest::getVersion() const {return _version;}

const std::string& HttpRequest::getQueries() const {return _queries;}

const std::string HttpRequest::getRoot() const { return _location->getRoot(); }

const std::string& HttpRequest::getCGIRes() const { return _cgiRes; };

std::map<std::string, std::string>& HttpRequest::getHeaders() {return _headers;}

std::map<std::string, std::string>& HttpRequest::getBody() {return _body;}

int	HttpRequest::getStatus() const {return _status;}

bool HttpRequest::getHeadersParsed() const {return _areHeadersParsed;}

bool HttpRequest::isCGIActive() const { return _isCGI; }

bool HttpRequest::getAutoIndex() const { return _location->getAutoIndex(); }

size_t	HttpRequest::getMaxBody() const { return _location->getClientMaxBodySize(); }

std::vector<std::string> HttpRequest::getArgv() const { return _argv; }

const std::vector<std::string> HttpRequest::getServerName() const { return _server->getServerName(); }

const std::vector<int> HttpRequest::getListenPort() const {return _server->getListenPort();}