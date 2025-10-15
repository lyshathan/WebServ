#include "CgiHandler.hpp"

/******************************************************************************/
/*						CGI FORK HELPERS									  */
/******************************************************************************/

const std::string	&CgiHandler::getResponse() const { return _finalResponse; }

void		CgiHandler::cgiInitEnv() {
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_SOFTWARE"] = "webserv/1.0";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	_env["REQUEST_METHOD"] = _client->httpReq->getMethod();
	_env["SCRIPT_NAME"] = _client->httpReq->getUri();
	_env["PATH_INFO"] = _client->httpReq->getUri();
	_env["REMOTE_ADDR"] = _client->getClientIp();
	_env["REMOTE_HOST"] = _client->getClientIp();

	// const std::vector<std::string> serverNames = _server->getServerName();
	// if (!serverNames.empty())
	// 	_env.push_back("SERVER_NAME=" + serverNames[0]);
	// else
	// 	_env.push_back("SERVER_NAME=localhost");

	// const std::vector<int> listenPort = _server->getListenPort();
	// ss << listenPort[0];
	// _env.push_back("SERVER_PORT=" + ss.str());

	// if (!_queries.empty())
	// 	_env.push_back("QUERY_STRING=" + _queries);
	// else
	// 	_env.push_back("QUERY_STRING=");

	// // Always calculate actual body length, don't trust headers for CGI
	// if (!_body.empty()) {
	// 	std::stringstream contentLengthSs;
	// 	// Calculate total body length from all body parts
	// 	size_t totalLength = 0;
	// 	for (std::map<std::string, std::string>::const_iterator bodyIt = _body.begin(); bodyIt != _body.end(); ++bodyIt) {
	// 		totalLength += bodyIt->second.length();
	// 	}
	// 	contentLengthSs << totalLength;
	// 	_env.push_back("CONTENT_LENGTH=" + contentLengthSs.str());
	// }
	// else {
	// 	_env.push_back("CONTENT_LENGTH=0");
	// }

	// it = _headers.find("content-type");
	// if (it != _headers.end())
	// 	_env.push_back("CONTENT_TYPE=" + it->second);
}

char **CgiHandler::getArgvArray() {
	const std::vector<std::string> &args = _client->httpReq->getArgv();
	size_t argc = args.size();

	char **argv = new char*[argc + 1];

	for (size_t i = 0; i < argc; ++i)
		argv[i] = strdup(args[i].c_str());
	argv[argc] = NULL;
	return argv;
}

char **CgiHandler::getEnvArray() {
	char **envp = new char*[_env.size() + 1];
	size_t i = 0;

	for (std::map<std::string, std::string>::iterator it = _env.begin(); it != _env.end(); ++it) {
		std::string entry = it->first + "=" + it->second;
		envp[i++] = strdup(entry.c_str());
	}
	envp[i] = NULL;
	return envp;
}

void CgiHandler::tryParseCGIHeaders() {
    if (_headersParsed)
        return;

    size_t headerEnd = _outputBuffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        headerEnd = _outputBuffer.find("\n\n");
        if (headerEnd != std::string::npos)
            headerEnd += 2;
    } else
        headerEnd += 4;

    _headerPos = headerEnd;

    if (headerEnd != std::string::npos) {
        parseCGIHeaders();
        _headersParsed = true;
    }
}

void CgiHandler::parseCGIHeaders() {
    std::string headers = _outputBuffer.substr(0, _headerPos);
    size_t start = 0;

    while (start < headers.length()) {
        size_t lineEnd = headers.find('\n', start);
        if (lineEnd == std::string::npos)
            break;

        std::string line = headers.substr(start, lineEnd - start);
        if (!line.empty() && line[line.length() - 1] == '\r')
            line.erase(line.length() - 1);

        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);

            while (!value.empty() && value[0] == ' ')
                value.erase(0, 1);
            while (!value.empty() && value[value.length() - 1] == ' ')
                value.erase(value.length() - 1);

            std::transform(key.begin(), key.end(), key.begin(), ::tolower);

            _cgiHeaders[key] = value;
        }
        start = lineEnd + 1;
    }

	std::map<std::string, std::string>::iterator it = _cgiHeaders.begin();

	for (; it != _cgiHeaders.end(); ++it) {
		std::cout << "[CGI HEADER] " << it->first << " : " << it->second << std::endl;
	}
}
