#include "HttpRequest.hpp"

/******************************************************************************/
/*						HANDLER HELPERS FUNCTIONS							  */
/******************************************************************************/

bool HttpRequest::validateMethods() {
	if (!_location) {
		_status = NOT_ALLOWED;
		return false;
	}
	std::vector<std::string> allowMethods = _location->getAllowMethods();
	std::vector<std::string>::iterator it = allowMethods.begin();

	if (_location && !allowMethods.empty()) {
		for (; it != allowMethods.end(); ++it) {
			if ((*it).compare(_method) == 0)
				return true;
		}
		_status = NOT_ALLOWED;
	}
	return false;
}

bool HttpRequest::setUri(std::string &path) {
	std::string filepath;
	if (!_location) {
		if (!_status)
			_status = NOT_FOUND;
		return false;
	}
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
	if (_location && _location->getAutoIndex())
		return true;
	if (!_status)
		_status = NOT_FOUND;
	return false;
}