#include "HttpResponse.hpp"

/******************************************************************************/
/*						PARSE  HELPER FUNCTIONS								  */
/******************************************************************************/

std::string	HttpResponse::getTime() const {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time (&rawtime);
	timeinfo = gmtime (&rawtime);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	return std::string(buffer);
}

bool	HttpResponse::isTextContent() {
	if (_mimeType.find("text/") == 0 || _mimeType == "application/json" ||
		_mimeType == "application/javascript") {
			_isTextContent = true;
			return 1;
	} else {
		_isTextContent = false;
		return 0;
	}

}

std::string HttpResponse::getLastModifiedTime() const {
	struct stat			file_stat;
	int					status;
	struct tm 			* timeinfo;
	char				buffer [80];

	status = stat(_request->getUri().c_str(), &file_stat);
	if (status != 0)
		return getTime();
	timeinfo = gmtime(&file_stat.st_mtime);
	strftime (buffer,80,"%a, %d %b %Y %H:%M:%S GMT", timeinfo);
	return std::string(buffer);
}
