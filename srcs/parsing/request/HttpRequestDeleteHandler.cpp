#include "HttpRequest.hpp"

/******************************************************************************/
/*								DELETE HANDLER								  */
/******************************************************************************/

bool HttpRequest::deleteHandler() {
	if (!isDeletePathValid()) {
		setErrorPage();
		return false;
	}
	deleteFile();
	if (!_status)
		_status = NO_CONTENT;
	return true;
}

bool HttpRequest::deleteFile() {
	if (unlink(_uri.c_str()) == 0) {
		return true;
	} else {
		_status = INTERNAL_ERROR;
		return false;
	}
}

bool HttpRequest::isDeletePathValid() {
	std::string	path = _location->getRoot() + _uri;

	struct stat buf;
	if (stat(path.c_str(), &buf) == 0) {
		if (S_ISREG(buf.st_mode)) {
			if (access(path.c_str(), R_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			_uri = path;
			return true;
		} else {
			_status = FORBIDDEN;
			return false;
		}
	} else {
		_status = NOT_FOUND;
		return false;
	}
}