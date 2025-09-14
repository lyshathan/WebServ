#include "HttpRequest.hpp"

/******************************************************************************/
/*							PARSE  GETTERS									  */
/******************************************************************************/

const std::string& HttpRequest::getMethod()const {return _method;}

const std::string& HttpRequest::getUri() const {return _uri;}

const std::string& HttpRequest::getVersion() const {return _version;}

std::map<std::string, std::string>& HttpRequest::getHeaders() {return _headers;}

int	HttpRequest::getStatus() const {return _status;}

bool HttpRequest::getHeadersParsed() const {return _areHeadersParsed;}