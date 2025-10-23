#include "HttpRequest.hpp"

/******************************************************************************/
/*							HANDLER FUNCTIONS								  */
/******************************************************************************/

void HttpRequest::requestHandler() {
	if (_uri.find("/.well-known/") == 0)
		return ;
	if (checkReturn()) {
		return;
	}
	if (isCGIPath()) {
		_status = CGI_PENDING;
		return;
	}
	else if (_method == "POST") {
		postHandler();
	}
	else if (_method == "GET") {
		getHandler();
	}
	else if (_method == "DELETE") {
		deleteHandler();
	}
}

void HttpRequest::getHandler() {
	isLocationPathValid();
	if (!_status)
		_status = OK;
}

bool HttpRequest::checkReturn() {
	if (!_location)
		return false;
	std::pair<int, std::string> returnCode = _location->getReturn();

	if (returnCode.first) {
		_status = returnCode.first;
		if (!returnCode.second.empty())
			_uri = returnCode.second;
		return true;
	}
	return false;
}

bool HttpRequest::isLocationPathValid() {
	std::string	path = _location->getRoot() + _uri;

	struct stat buf;
	if (_location && !stat(path.c_str(),&buf)) {
		if (S_ISREG(buf.st_mode)) {
			if (access(path.c_str(),  R_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			_uri = path;
			return true;
		}
		else if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/') {
				_uri += "/";
				_status = MOVED_PERMANENTLY;
				return false;
			}
			if (!setUri(path))
				return false;
			return true;
		} else {
			_status = FORBIDDEN;
			return false;
		}
	}
	if (!_status)
		_status = NOT_FOUND;
	return false;
}