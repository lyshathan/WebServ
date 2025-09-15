#include "HttpRequest.hpp"

/******************************************************************************/
/*							LOCATION FUNCTIONS								  */
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
	if (_method == "POST") {
		if (!isUploadPathValid()) {
			setErrorPage();
			return;
		}
	} else {
		if (!isLocationPathValid()) {
		setErrorPage();
		return;
	}
	}
	if (!_status)
		_status = OK;
}

bool HttpRequest::validateMethods() {
	std::vector<std::string> allowMethods = _location->getAllowMethods();
	std::vector<std::string>::iterator it = allowMethods.begin();

	if (!allowMethods.empty()) {
		for (; it != allowMethods.end(); ++it) {
		if ((*it).compare(_method) == 0)
			return true;
		}
		std::cout << "FORBIDDEN\n";
		_status = FORBIDDEN;
	}
	return false;
}

bool HttpRequest::setUri(std::string &path) {
	std::string filepath;
	std::vector<std::string> index = _location->getIndex();

	// std::cout << "setUri path\n";
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
	if (!_status)
		_status = NOT_FOUND;
	return false;
}

bool HttpRequest::isUploadPathValid() {
	std::string	path = _location->getUploadPath() + _uri;

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
	}

	// std::cout << "Body " << _body << "\n";
	std::string fullPath = "./uploaded_body.png";

	// 3. Open file in binary mode
	// std::ofstream file(fullPath, std::ios::binary);
	// if (!file.is_open()) {
	// 	std::cout << "Error opening the file\n";
	// 	return false; // Error opening file
	// }
	// file.write(body.data(), body.size());
	// if (file.fail()) {
	// 	return false;
	// }

	// file.close();

	return true;
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

void HttpRequest::pickLocationConfig() {
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
	} else
		_location = &(*locations.begin());
}