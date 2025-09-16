#include "HttpRequest.hpp"

/******************************************************************************/
/*							HANDLER FUNCTIONS								  */
/******************************************************************************/

void HttpRequest::requestHandler() {
	std::cout << "Request incoming from .." << _uri << "\n";
	if (_uri.find("/.well-known/") == 0)
		return ;
	pickServerConfig();
	pickLocationConfig();
	if (!validateMethods()) {
		setErrorPage();
		return;
	}
	if (_method == "POST" && !postHandler()) {
		return;
	} else if (_method == "GET" && !getHandler()) {
		return;
	}
}

bool	HttpRequest::postHandler() {
	if (!isUploadPathValid()) {
		setErrorPage();
		return false;
	}
	createFile();
	if (!_status)
		_status = CREATED;
	return true;
}

bool	HttpRequest::getHandler() {
	if (!isLocationPathValid()) {
		setErrorPage();
		return false;
	}
	if (!_status)
		_status = OK;
	return true;
}

bool HttpRequest::isUploadPathValid() {
	std::string	path = _location->getUploadPath() + _uri;
	struct stat buf;

	std::cout << "Upload Path " << path << "\n";
	if (!stat(path.c_str(),&buf)) {
		if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/') {
				_uri += "/";
				_status = MOVED_PERMANENTLY;
				return false;
			}
			return true;
		}
	}
	if (!_status)
		_status = NOT_FOUND;
	return false;
}

bool HttpRequest::isLocationPathValid() {
	std::string	path = _location->getRoot() + _uri;

	std::cout << "Path " << path << "\n";
	struct stat buf;
	if (!stat(path.c_str(),&buf)) {
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
		}
	}
	if (!_status)
		_status = NOT_FOUND;
	return false;
}