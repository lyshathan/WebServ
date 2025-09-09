#include "HttpRequest.hpp"

/******************************************************************************/
/*							LOCATION FUNCTIONS								  */
/******************************************************************************/

void HttpRequest::requestHandler() {
	if (pickServerConfig() || !pickLocationConfig() || !isLocationPathValid())
		return ;
	_status = OK;
}

bool HttpRequest::setUri(std::string &path) {
	std::string filepath;
	std::vector<std::string> index = _location->getIndex();

	if (!index.empty()) {
		std::vector<std::string>::iterator it = index.begin();
		for (; it != index.end(); ++it) {
			filepath = path + *it;
			if (access(filepath.c_str(),  F_OK) == 0) {
				if (access(filepath.c_str(),  R_OK) == 0) {
					_uri = filepath;
					return true;
				}
				_status = FORBIDDEN;
				return false;
			}
		}
	}
	_status = NOT_FOUND;
	setErrorPage();
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
			return true;
		}
		else if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/') {
				_status = MOVED_PERMANENTLY;
				return false;
			}
			if (!setUri(path))
				return false;
			return true;
		}
	} else {
		_status = NOT_FOUND;
		setErrorPage();
		return false;
	}
	return false;
}

bool HttpRequest::pickLocationConfig() {
	const std::vector<LocationConfig> &locations = _server->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::vector<LocationConfig>::const_iterator bestMatch = locations.end();
	size_t longestMatch = 0;

	for (; it != locations.end(); ++it) {
		std::string	path = it->getPath();

		if (_uri.length() >= path.length()) {
			if (it->getExactMatch()) {
				if (path == _uri) {
					bestMatch = it;
					break ;
				}
			}
			else if (_uri.compare(0, path.length(), path) == 0) {
				if (path.length() > longestMatch) {
					longestMatch = path.length();
					bestMatch = it;
				}
			}
		}
	}
	if (bestMatch != locations.end()) {
		_location = &(*bestMatch);
		std::cout << "Location chosen " << _location->getPath() << "\n";
		return true;
	}
	_status = NOT_FOUND;
	setErrorPage();
	return false;
}