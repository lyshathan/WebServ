#include "HttpRequest.hpp"
#include <cctype>

/******************************************************************************/
/*								CGI HELPERS									  */
/******************************************************************************/

bool HttpRequest::isCGIPath() {
	struct stat buf;
	std::string	path = _location->getRoot() + _uri;
	std::string tmp = _uri;

	if (!stat(path.c_str(),&buf)) {
		if (S_ISREG(buf.st_mode)) {
			if (access(path.c_str(),  R_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			_uri = path;
			if (checkCGI()) {
				_isCGI = true;
				return true;
			}
			_uri = tmp;
		}
		else if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/')
				path += "/";
			if (setUri(path) && checkCGI()) {
				_isCGI = true;
				return  true;
			}
			else
				_uri = tmp;
		}
	}
	return false;
}

bool	HttpRequest::checkCGI() {
	std::map<std::string, std::string>	cgiData = _location->getCGIData();

	if (!cgiData.empty()) {
		size_t pos = _uri.find_last_of(".");
		if (pos != std::string::npos) {
			std::string extension = _uri.substr(pos);
			std::map<std::string, std::string>::iterator it = cgiData.begin();
			for (; it != cgiData.end(); ++it) {
				if (extension.compare(it->first) == 0) {
					_argv.push_back(it->second);
					_argv.push_back(_uri);
					return true;
				}
			}
		}
	}
	return false;
}