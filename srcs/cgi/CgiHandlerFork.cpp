#include "CgiHandler.hpp"

/******************************************************************************/
/*							CGI FORK										  */
/******************************************************************************/

void		CgiHandler::cgiStart() {
	int		stdinFd[2];
	int		stdoutFd[2];
	_client->cgiInitEnv();

	if (pipe(stdinFd) < 0 || pipe(stdoutFd) < 0) {
		_client->httpReq->setStatus(INTERNAL_ERROR);
		return ;
	}

	fcntl(stdinFd[1], F_SETFL, O_NONBLOCK);
	fcntl(stdoutFd[0], F_SETFL, O_NONBLOCK);

	_pid = fork();

	if (_pid < 0) {
		close(stdinFd[0]);
		close(stdinFd[1]);
		close(stdoutFd[0]);
		close(stdoutFd[1]);
		_client->httpReq->setStatus(INTERNAL_ERROR);
		return ;
	}
	if (_pid == 0)
		handleChild(stdinFd, stdoutFd);
	else if (_pid > 0)
		handleParent(stdinFd, stdoutFd);
}

void		CgiHandler::handleChild(int stdinFd[2], int stdoutFd[2]) {
	if (dup2(stdinFd[0], STDIN_FILENO) < 0)
		exit(1);
	if (dup2(stdoutFd[1], STDOUT_FILENO) < 0)
		exit(1);
	close(stdinFd[0]);
	close(stdinFd[1]);
	close(stdoutFd[0]);
	close(stdoutFd[1]);
	char** argv = getArgvArray();
	char** envp = getEnvArray();
	if (execve(argv[0], argv, envp) == -1)
		exit(EXIT_FAILURE);
}

void		CgiHandler::handleParent(int stdinFd[2], int stdoutFd[2]) {
	_stdinFd = stdinFd[1];
	_stdoutFd = stdoutFd[0];
	_headerPos = 0;
	_headersParsed = false;

	close(stdinFd[0]);
	close(stdoutFd[1]);

	std::map<std::string, std::string> body = _client->httpReq->getBody();
	if (!body.empty()) {
		_inputBuffer = "";
		for (std::map<std::string, std::string>::const_iterator it = body.begin(); it != body.end(); ++it)
			_inputBuffer += it->second;
		_bytesWritten = 0;
		_cgiStage = CGI_WRITING;
	} else {
		close(_stdinFd);
		_stdinFd = -1;
		_cgiStage = CGI_READING;
	}
}
