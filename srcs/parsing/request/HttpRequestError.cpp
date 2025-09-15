#include "HttpRequest.hpp"

void HttpRequest::setErrorPage() {
	std::map<int, std::string> path = _location->getErrorPages();
	std::string errorPath = _location->getRoot();

	std::cout << "Looking for error pages..\n";
	if (!path.empty()) {
		std::map<int, std::string>::iterator it = path.find(_status);
		if (it != path.end()) {
			std::string errorPath = it->second;
			if (errorPath[0] == '/') {
				_method = "GET";
				std::cout << "Error found " << it->second << "\n";
				_uri = it->second;
				requestHandler();
			} else {
				_status = MOVED_PERMANENTLY_302;
				_uri = it->second;
			}
		}
	}
}