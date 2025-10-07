#include "HttpResponse.hpp"

/******************************************************************************/
/*								GETTERS										  */
/******************************************************************************/

const std::string& HttpResponse::getRes() const {return _res;}
const std::vector<char>& HttpResponse::getBinRes() const {return _binRes;}
bool	HttpResponse::getIsTextContent() const {return _isTextContent;}

const std::string& HttpResponse::getResHeaders() {
	std::ostringstream	headers;

	headers << _responseStatus;
	std::map<std::string, std::string>::iterator it = _headers.begin();
	for (; it != _headers.end(); ++it)
		headers << it->first << it->second;
	headers << "\r\n";
	_resHeaders = headers.str();
	return _resHeaders;
}

std::string HttpResponse::getMimeType() const {
	std::string uri = _request->getUri();

	size_t pos = uri.find_last_of(".");
	if (pos == std::string::npos) {
		return "text/html";
	}
	else {
		std::string extension = uri.substr(pos + 1);
		if (extension == "html" || extension == "htm")
			return "text/html";
		else if (extension == "css")
			return "text/css";
		else if (extension == "js")
			return "text/javascript";
		else if (extension == "json")
			return "application/json";
		else if (extension == "png")
			return "image/png";
		else if (extension == "jpg" || extension == "jpeg")
			return "image/jpeg";
		else if (extension == "gif")
			return "image/gif";
		else if (extension == "svg")
			return "image/svg+xml";
		else if (extension == "ico")
			return "image/x-icon";
		else if (extension == "txt")
			return "text/plain";
		else if (extension == "xml")
			return "text/xml";
		else if (extension == "pdf")
			return "application/pdf";
	}
	return "application/octet-stream";
}

std::map<std::string, UserData>& HttpResponse::getSessions() {
	return _sessions;
}