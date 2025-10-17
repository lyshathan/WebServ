#include "HttpRequest.hpp"

void HttpRequest::setErrorPage() {
	if (_isProccessingError)
		return ;
	_isProccessingError = true;
	if (!_location)
		return ;
	std::map<int, std::string> path = _location->getErrorPages();
	std::string errorPath = _location->getRoot();

	if (!path.empty()) {
		std::map<int, std::string>::iterator it = path.find(_status);
		if (it != path.end()) {
			std::string errorPath = it->second;
			if (errorPath[0] == '/') {
				_method = "GET";
				_uri = it->second;
				pickLocationConfig();
				requestHandler();
			} else {
				_status = MOVED_PERMANENTLY_302;
				_uri = it->second;
			}
		}
	}
}