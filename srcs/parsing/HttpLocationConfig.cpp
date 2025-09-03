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
	std::string filepath;

	if (pickServerConfig() || !pickLocationConfig())
		return false;

	filepath = _location->getRoot() + _uri;
	if (!_uri.empty() && _uri[_uri.length() - 1] == '/') {
		std::vector<std::string> files = _location->getIndex();
		std::vector<std::string>::iterator it = files.begin();
		for (; it != files.end(); ++it) {
			if (access((filepath + *it).c_str(),  F_OK | R_OK) == 0) {
				_uri = filepath + *it;
				//std::cout << "Filepath - " << filepath + *it << "\n";
				return true;
			}
		}
		_status = NOT_FOUND;
		return false;
	}
	std::cout << "File path = " << filepath << "/n";
	if (access(filepath.c_str(),  F_OK | R_OK) != 0) return false;
	_uri = filepath;
	std::cout << "Filepath - " << filepath << "\n";
	return true;
}

bool	is_directory(const char *path) {
	struct stat info;
	if (stat(path, &info) !=0) return false;
	return (info.st_mode & S_IFDIR) !=0;
}

bool HttpRequest::isLocationValid() {
	std::vector<LocationConfig> locations = _server->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::vector<LocationConfig>::const_iterator bestMatch = locations.end();
	size_t longestMatch = 0;

	for (; it != locations.end(); ++it) {
		std::string path = it->getPath();
		if (!path.empty() && path[path.length() - 1] == '/')
			path.resize(path.size() - 1);
		if (_uri.compare(0, path.length(), path) == 0)
		if (it->getPath().find(_uri) != std::string::npos) {
			if (path.length() > longestMatch) {
					longestMatch = path.length();
					bestMatch = it;
			}
		}
	}
	if (bestMatch != locations.end()) {
		std::cout << "Location " << bestMatch->getPath() << "\n";
		return true;
	}
	return false;
}

bool HttpRequest::pickLocationConfig() {
	const std::vector<LocationConfig> &locations = _server->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::vector<LocationConfig>::const_iterator bestMatch = locations.end();
	size_t longestMatch = 0;

	if (!_uri.empty() && _uri[_uri.length() - 1] != '/') {
			if (isLocationValid()) {
			_status = MOVED_PERMANENTLY;
			std::cout << "Moved Permanently\n";
			} else {
				_status = NOT_FOUND;
			}
			return false;
	}
	for (; it != locations.end(); ++it) {
		if (_uri.compare(0, it->getPath().length(), it->getPath()) == 0) {
				if (it->getPath().length() > longestMatch) {
					longestMatch = it ->getPath().length();
					bestMatch = it;
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