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

	//Selects correct server config and set it
	if (pickServerConfig())
		return false;
	pickLocationConfig();
	
	if (_location) {
		filepath = _location->getRoot() + _uri;
	} else {
		return false;
	}
	std::cout << "Location " << _location->getPath() << "\n";
	//std::cout << "File Path " << filepath << "\n";
	// if (!_uri.empty() && _uri[_uri.length() - 1] == '/') {
	// 	std::vector<std::string> files = _location->getIndex();
	// 	if (!files.empty()) {
	// 		filepath +=  files.front();
	// 	}
	// }
	// //std::cout << "File Path " << filepath << "\n";
	if (access(filepath.c_str(),  F_OK | R_OK) != 0) return false;
	_uri = filepath;
	//std::cout << "File Path " << _uri << "\n";
	return true;
}

bool HttpRequest::isLocationValid(std::string uri) {
	std::vector<LocationConfig> locations = _server->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::string filepath = uri + "/";

	for (; it != locations.end(); ++it) {
		if (it->getPath().find(filepath) != std::string::npos)
			return true;
	}
	return false;
}

void HttpRequest::pickLocationConfig() {
    //Pick correct server config
    //Set it at _server
	const std::vector<LocationConfig> &locations = _server->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::vector<LocationConfig>::const_iterator bestMatch = locations.end();
	size_t longestMatch = 0;

	if (!_uri.empty() && _uri[_uri.length() - 1] != '/' && isLocationValid(_uri)) {
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