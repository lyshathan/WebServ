#include "HttpRequest.hpp"

/******************************************************************************/
/*							LOCATION FUNCTIONS								  */
/******************************************************************************/

bool HttpRequest::validateUri() {
	if (_uri[0] != '/') return false;
	if (_uri.find("..") != std::string::npos) return false;
	if (_uri.find('\0') != std::string::npos) return false;
	for (size_t i = 0; i < _uri.length(); ++i) {
		if (!std::isalnum(_uri[i])) {
			if (_uri[i] == '/' || _uri[i] == '.' || _uri[i] == '-' || _uri[i] == '_')
				continue;
			return false;
		}
	}
	return true;
}

bool HttpRequest::validatePath() {
	if (pickServerConfig() || !pickLocationConfig() || !isLocationPathValid())
		return false;
	return true;
}

bool HttpRequest::setUri(std::string &path) {
	std::string filepath;
	std::vector<std::string> index = _location->getIndex();

	if (!index.empty()) {
		std::vector<std::string>::iterator it = index.begin();
		for (; it != index.end(); ++it) {
			filepath = path + *it;
			std::cout << "File to check " << filepath << "\n";
			if (access(filepath.c_str(),  F_OK) == 0) {
				if (access(filepath.c_str(),  R_OK) == 0) {
					std::cout << "Default index found\n"
					<< filepath << "\n";
					_uri = filepath;
					return true;
				}
				_status = FORBIDDEN;
				std::cout << "Path is a valid file but there's no access to it\n";
				return false;
			}
		}
	}
	std::cout << "Path does not exist\n";
	_status = NOT_FOUND;
	return false;
}

bool HttpRequest::isLocationPathValid() {
	std::string	path = _location->getRoot() + _uri;

	struct stat buf;
	if (!stat(path.c_str(),&buf)) {
		if (S_ISREG(buf.st_mode)) {
			if (access(path.c_str(),  R_OK) != 0) {
				std::cout << "Path is a valid file but there's no access to it\n";
				_status = FORBIDDEN;
				return false;
			}
			std::cout << "Path is a valid file\n";
			return true;
		}
		else if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				std::cout << "Path is a dir file but there's no access to it\n";
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/') {
				_status = MOVED_PERMANENTLY;
				return false;
			}
			if (!setUri(path))
				return false;
			std::cout << "Path is a valid dir\n";
			return true;
		}
	} else {
		std::cout << "Path does not exist\n";
		_status = NOT_FOUND;
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
		return true;
	}
	_status = NOT_FOUND;
	return false;
}