#include "HttpRequest.hpp"

/******************************************************************************/
/*								CGI HELPERS									  */
/******************************************************************************/

bool HttpRequest::isCGIPath() {
	struct stat buf;
	std::string	path = _location->getRoot() + _uri;
	std::string tmp = _uri;

	if (!stat(path.c_str(),&buf)) {
		if (S_ISREG(buf.st_mode)) {
			if (access(path.c_str(),  R_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			_uri = path;
			if (checkGGI()) {
				_isCGI = true;
				return true;
			}
			_uri = tmp;
		}
		else if (S_ISDIR(buf.st_mode)) {
			if (access(path.c_str(),  R_OK | X_OK) != 0) {
				_status = FORBIDDEN;
				return false;
			}
			if (!_uri.empty() && _uri[_uri.length() - 1] != '/')
				path += "/";
			if (setUri(path) && checkGGI()) {
				_isCGI = true;
				return  true;
			}
			else
				_uri = tmp;
		}
	}
	return false;
}

bool	HttpRequest::checkGGI() {
	std::string cgiPath = _location->getCGIPath();
	std::string cgiExtension = _location->getCGIExtension();

	if (cgiPath.empty() || cgiExtension.empty())
		return false;

	size_t pos = _uri.find_last_of(".");
	if (pos != std::string::npos) {
		std::string extension = _uri.substr(pos);
		if (extension.compare(cgiExtension) == 0) {
			_argv.push_back(cgiPath);
			_argv.push_back(_uri);
			return true;
		}
	}
	return false;
}

char** HttpRequest::getArgvArray() {
	static std::vector<char*> argv_ptrs;
	argv_ptrs.clear();

	for (size_t i = 0; i < _argv.size(); ++i) {
		argv_ptrs.push_back(const_cast<char*>(_argv[i].c_str()));
	}
	argv_ptrs.push_back(NULL);

	return &argv_ptrs[0];
}

char** HttpRequest::getEnvArray() {
	static std::vector<char*> env_ptrs;
	env_ptrs.clear();

	for (size_t i = 0; i < _env.size(); ++i) {
		env_ptrs.push_back(const_cast<char*>(_env[i].c_str()));
	}
	env_ptrs.push_back(NULL);

	return &env_ptrs[0];
}

void	HttpRequest::initEnv() {
	std::map<std::string, std::string>::iterator it = _headers.begin();
	std::stringstream ss;

	_env.push_back("PATH_INFO=" + _uri);
	_env.push_back("SCRIPT_NAME=" + _uri);
	_env.push_back("REQUEST_METHOD=" + _method);
	it = _headers.find("content-length");
	if (it != _headers.end())
		_env.push_back("CONTENT_LENGTH=" + it->second);
	it = _headers.find("content-type");
	if (it != _headers.end())
		_env.push_back("CONTENT_TYPE=" + it->second);
	const std::vector<std::string> serverNames = _server->getServerName();
	if (!serverNames.empty())
		_env.push_back("SERVER_NAME=" + serverNames[0]);
	const std::vector<int> listenPort = _server->getListenPort();
	ss << listenPort[0];
	if (!serverNames.empty())
		_env.push_back("SERVER_PORT=" + (ss.str()));
	if (!_queries.empty())
		_env.push_back("QUERY_STRING=" + _queries);
}
