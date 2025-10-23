#include "HttpResponse.hpp"

/******************************************************************************/
/*								GETTERS										  */
/******************************************************************************/

const std::string& HttpResponse::getRes() const {return _res;}

int HttpResponse::getResStatus() const {return _status;}

const std::vector<char>& HttpResponse::getBinRes() const {return _binRes;}

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

std::string HttpResponse::getMimeType(std::string path) const {
    if (_status == 201) {
        return "text/html";
    }
	if (_status >= 300)
        return "text/html";

    if (!path.empty() && path[path.size() - 1] == '/')
        return "text/html";

    size_t pos = path.find_last_of('.');
    if (pos == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(pos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    std::map<std::string, std::string>::const_iterator it = _mimeTypes.find(ext);
    if (it != _mimeTypes.end())
        return it->second;
    return "application/octet-stream";
}


std::map<std::string, UserData>& HttpResponse::getSessions() {
	return _sessions;
}