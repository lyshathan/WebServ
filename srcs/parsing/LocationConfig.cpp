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

bool HttpRequest::isLocationValid(std::string uri) {
	std::vector<LocationConfig> locations = _config->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::string filepath = uri + "/";

	for (; it != locations.end(); ++it) {
		if (it->getPath().find(filepath) != std::string::npos)
			return true;
	}
	return false;
}

bool HttpRequest::validatePath() {
	std::string filepath;
	pickServerConfig();
	if (_location) {
		filepath = _location->getRoot() + _uri;
		_location->PrintLocation();
	} else {
		return false;
	}
	//if (_uri == "/") filepath = "./www/index.html";
	if (access(filepath.c_str(),  F_OK | R_OK) != 0) return false;
	_uri = filepath;
	return true;
}

void HttpRequest::pickServerConfig() {
	const std::vector<LocationConfig> &locations = _config->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::vector<LocationConfig>::const_iterator bestMatch = locations.end();
	size_t longestMatch = 0;

	if (_uri[_uri.length() - 1] != '/' && isLocationValid(_uri)) {
		_status = MOVED_PERMANENTLY;
		std::cout << "Moved Permanently\n";
		return;
	}
	for (; it != locations.end(); ++it) {
		if (_uri.compare(0, it->getPath().length(), it->getPath()) == 0) {
				if (it->getPath().length() > longestMatch) {
					longestMatch = it ->getPath().length();
					bestMatch = it;
				}
		}
	}
	if (bestMatch != locations.end())
		_location = &(*bestMatch);
}