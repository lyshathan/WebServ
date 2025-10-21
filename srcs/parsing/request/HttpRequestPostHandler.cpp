#include "HttpRequest.hpp"

/******************************************************************************/
/*								POST HANDLER								  */
/******************************************************************************/

bool HttpRequest::postHandler() {
	if (!_location || !isUploadPathValid()) {
		setErrorPage();
		return false;
	}
	if (_uri.find("/cookie") == std::string::npos && _status < 400)
		createFile();
	if (!_status)
		_status = CREATED;
	return true;
}

bool HttpRequest::isUploadPathValid() {
	if (!_location) {
		if (!_status)
			_status = NOT_FOUND;
		return false;
	}
	std::string	path = _location->getUploadPath() + _uri;
	struct stat buf;

	// clear// std::cerr << "Upload Path " << path << "\n";
	if (_location && !stat(path.c_str(),&buf)) {
		if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/') {
				_uri += "/";
				return true;
			}
			return true;
		}
	}
	if (!_status)
		_status = NOT_FOUND;
	return false;
}

bool HttpRequest::nameAttribute(std::string& path, std::string& filename) {
	int counter = 1;
	std::string originalFilename = filename;
	std::string filepath = path + _uri + filename;

	size_t dotPos = originalFilename.find_last_of('.');
	std::string baseName, extension;

	if (dotPos != std::string::npos) {
		baseName = originalFilename.substr(0, dotPos);
		extension = originalFilename.substr(dotPos);
	} else {
		baseName = originalFilename;
		extension = "";
	}

	while (access(filepath.c_str(), F_OK) == 0) {
		std::stringstream ss;
		ss << counter;
		filename = baseName + "_" + ss.str() + extension;
		filepath = path + _uri + filename;
		counter++;
		if (counter > 9999)
			return false;
	}
	path = filepath;
	return true;
}

void HttpRequest::generateName(std::string &finalName) {
	std::ostringstream filename;
	setExtensions();

	std::string extension = ".bin";

	std::map<std::string, std::string>::iterator it = _headers.find("content-type");
	if (it != _headers.end()) {
		std::string contentType = it->second;
		std::transform(contentType.begin(), contentType.end(), contentType.begin(), ::tolower);

		if (_extensions.count(contentType))
			extension = _extensions[contentType];
		else if (contentType.find("text/") == 0 || contentType.find("application/x-www-form-urlencoded") != std::string::npos)
			extension = ".txt";
	}

	filename << "upload_" << std::time(NULL) << extension;
	finalName = filename.str();
}


bool HttpRequest::createFile() {
	std::string path = _location->getUploadPath();

	if (!_location)
		return false;

	std::map<std::string, std::string>::iterator it = _body.begin();
	for (; it != _body.end(); ++it) {
		std::string filepath = path;
		std::string filename = it->first;
		if (filename.empty())
			generateName(filename);
		if (!nameAttribute(filepath, filename)) return false;
		std::ofstream file(filepath.c_str(), std::ios::binary);
		if (!file.is_open()) return false;
		file.write(it->second.data(), it->second.size());
		if (file.fail()) {
			return false;
		}
		file.close();
	}
	return true;
}