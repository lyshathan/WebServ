#include "HttpRequest.hpp"

/******************************************************************************/
/*							PARSE  GETTERS									  */
/******************************************************************************/

const std::string& HttpRequest::getMethod()const {return _method;}

const std::string& HttpRequest::getUri() const {return _uri;}

const std::string& HttpRequest::getVersion() const {return _version;}

const std::string HttpRequest::getRoot() const { return _location->getRoot(); }

std::map<std::string, std::string>& HttpRequest::getHeaders() {return _headers;}

int	HttpRequest::getStatus() const {return _status;}

bool HttpRequest::getHeadersParsed() const {return _areHeadersParsed;}

bool HttpRequest::getAutoIndex() const { return _location->getAutoIndex(); }

void HttpRequest::setHeadersParsed() { _areHeadersParsed = false; }

void HttpRequest::setExtensions() {
	_extensions["image/jpeg"] = ".jpg";
	_extensions["image/png"] = ".png";
	_extensions["image/gif"] = ".gif";
	_extensions["application/pdf"] = ".pdf";
	_extensions["text/plain"] = ".txt";
	_extensions["application/octet-stream"] = ".bin";
}