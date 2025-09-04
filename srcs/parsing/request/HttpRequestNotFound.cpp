#include "HttpRequest.hpp"

void HttpRequest::setErrorPage() {
	std::map<int, std::string> path = _location->getErrorPages();
	std::string errorPath = _location->getRoot();

	if (!path.empty()) {
		std::map<int, std::string>::iterator it = path.find(_status);
		if (it != path.end()) {
			std::cout << "Error path " << it->second << "\n";
			errorPath += it->second;
			if (access(errorPath.c_str(), F_OK | R_OK) == 0) {
				_uri = errorPath;
			}
		}
	}
}