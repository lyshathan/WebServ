#include "HttpRequest.hpp"

void HttpRequest::setErrorPage() {
	std::map<int, std::string> path = _location->getErrorPages();
	std::string errorPath = _location->getRoot();

	std::cout << "Looking for error pages..\n";
	if (!path.empty()) {
		std::map<int, std::string>::iterator it = path.find(_status);
		if (it != path.end()) {
			errorPath = errorPath + _location->getPath() + it->second;
			std::cout << "Error path " << errorPath << "\n";
			if (access(errorPath.c_str(), F_OK | R_OK) == 0) {
				_uri = errorPath;
				std::cout << "Error path found " << _uri << "\n";
			}
		}
	}
}