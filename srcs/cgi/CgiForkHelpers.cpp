#include "CgiHandler.hpp"

/******************************************************************************/
/*						GETTERS/SETTERS										  */
/******************************************************************************/

int	CgiHandler::getStdinFd () const { return _stdinFd; }

int	CgiHandler::getStdoutFd () const { return _stdoutFd; }

bool CgiHandler::headersParsed () const { return _headersParsed; }

std::map<std::string, std::string> CgiHandler::getCgiHeaders() const { return _cgiHeaders; }

CgiState CgiHandler::getCgiStage () const { return _cgiStage; }

pid_t	CgiHandler::getPid() const { return _pid; }

/******************************************************************************/
/*						CGI FORK HELPERS									  */
/******************************************************************************/

const std::string	&CgiHandler::getResponse() const { return _finalResponse; }

char **CgiHandler::getArgvArray() {
	std::vector<std::string> args = _client->httpReq->getArgv();
	size_t argc = args.size();
	char **argv = new char*[argc + 1];

	for (size_t i = 0; i < argc; ++i)
		argv[i] = strdup(args[i].c_str());
	argv[argc] = NULL;
	return argv;
}

char **CgiHandler::getEnvArray() {
	std::map<std::string, std::string> env = _client->getCgiEnv();
	if (env.empty()) {
		char **envp = new char*[1];
		envp[0] = NULL;
		return envp;
	}

	char **envp = new char*[env.size() + 1];
	size_t i = 0;

	for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
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
}
