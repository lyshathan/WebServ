#include "HttpRequest.hpp"

/******************************************************************************/
/*							PARSE  SETTERS									  */
/******************************************************************************/

void HttpRequest::setHeadersParsed() { _areHeadersParsed = false; }

void HttpRequest::setExtensions() {
	_extensions["image/jpeg"] = ".jpg";
	_extensions["image/png"] = ".png";
	_extensions["image/gif"] = ".gif";
	_extensions["application/pdf"] = ".pdf";
	_extensions["text/plain"] = ".txt";
	_extensions["application/octet-stream"] = ".bin";
}

void HttpRequest::setStatus(int status) { _status = status;}

void HttpRequest::setCGIResult(const std::string &result) {
	_cgiRes = result;
}