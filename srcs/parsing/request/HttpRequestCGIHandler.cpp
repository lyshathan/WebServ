#include "HttpRequest.hpp"

/******************************************************************************/
/*								CGI HANDLER									  */
/******************************************************************************/

void	HttpRequest::cgiHandler() {
	initEnv();
	executeBin();

	if (!_status)
		_status = OK;
}

void	HttpRequest::childHandler(int stdin_fd[2], int stdout_fd[2]) {
	if (dup2(stdin_fd[0], STDIN_FILENO) < 0)
		exit(1);
	if (dup2(stdout_fd[1], STDOUT_FILENO) < 0)
		exit(1);
	close(stdin_fd[0]);
	close(stdin_fd[1]);
	close(stdout_fd[0]);
	close(stdout_fd[1]);
	char** argv = getArgvArray();
	char** envp = getEnvArray();
	if (execve(argv[0], argv, &envp[0]) == -1)
		exit(1);
}

void HttpRequest::parentHandler(int stdin_fd[2], int stdout_fd[2], pid_t pid) {
	int		status;
	size_t	length = 0;

	std::map<std::string, std::string>::iterator it = _headers.find("content-length");
	if (it != _headers.end()) {
		std::istringstream iss(it->second);
		size_t contentLength = 0;
		iss >> contentLength;
		length = contentLength;
	}

	close(stdin_fd[0]);
	close(stdout_fd[1]);

	if (!_body.empty()) {
		std::string firstValue = _body.begin()->second;
		if (length > 0 && !firstValue.empty()) {
			it = _headers.find("content-type");
			if (it != _headers.end()) {
				size_t actualLength = std::min(length, firstValue.length());
				write(stdin_fd[1], firstValue.c_str(), actualLength);
			}
		}
	}
	close(stdin_fd[1]);

	if (waitpid(pid, &status, 0) < 0) {
		_status = INTERNAL_ERROR;
		return ;
	}
	if (WIFEXITED(status)) {
		int exit_code = WEXITSTATUS(status);
		if (exit_code != 0) {
			_status = INTERNAL_ERROR;
			return ;
		}
	} else if (WIFSIGNALED(status)) {
		_status = INTERNAL_ERROR;
		return ;
	}
	readBuffer(stdout_fd);
	close(stdout_fd[0]);
}

void	HttpRequest::readBuffer(int stdout_fd[2]) {
	char buffer[4096];
	ssize_t bytesRead;

	while ((bytesRead = read(stdout_fd[0], buffer, sizeof(buffer))) > 0)
		_cgiRes.append(buffer, bytesRead);
	if (bytesRead < 0)
		_status = INTERNAL_ERROR;
}

void	HttpRequest::executeBin() {
	int		stdin_fd[2];
	int		stdout_fd[2];
	pid_t	pid;

	if (pipe(stdin_fd) < 0 || pipe(stdout_fd) < 0) {
		_status = INTERNAL_ERROR;
		return ;
	}
	pid = fork();
	if (pid < 0) {
		_status = INTERNAL_ERROR;
		return ;
	}
	if (pid == 0)
		childHandler(stdin_fd, stdout_fd);
	else if (pid > 0)
		parentHandler(stdin_fd, stdout_fd, pid);
}