#include "HttpRequest.hpp"

/******************************************************************************/
/*								POST HANDLER								  */
/******************************************************************************/

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

bool HttpRequest::createFile() {
	std::string path = _location->getUploadPath();

	std::map<std::string, std::string>::iterator it = _body.begin();
	for (; it != _body.end(); ++it) {
		std::string filepath = path;
		std::string filename = it->first;
		if (filename.empty())
			filename = "Webserv";
		if (!nameAttribute(filepath, filename)) return false;
		std::ofstream file(filepath, std::ios::binary);
		if (!file.is_open()) return false;
		file.write(it->second.data(), it->second.size());
		if (file.fail()) {
			return false;
		}
		file.close();
	}
	return true;
}