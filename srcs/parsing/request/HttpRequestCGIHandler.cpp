#include "HttpRequest.hpp"
#include <fcntl.h>

/******************************************************************************/
/*								CGI HANDLER									  */
/******************************************************************************/

void	HttpRequest::cgiHandler() {
	initEnv();
	_cgiState = new CgiState();
	_status = CGI_PENDING;
	executeBin();
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
	if (execve(argv[0], argv, envp) == -1)
		exit(1);
}

void HttpRequest::parentHandler(int stdin_fd[2], int stdout_fd[2], pid_t pid) {
	_cgiState->pid = pid;
	_cgiState->stdin_fd = stdin_fd[1];
	_cgiState->stdout_fd = stdout_fd[0];
	_cgiState->start_time = time(NULL);

	close(stdin_fd[0]);
	close(stdout_fd[1]);
	if (!_body.empty()) {
		// Combine all body parts into request_body
		_cgiState->request_body = "";
		for (std::map<std::string, std::string>::const_iterator it = _body.begin(); it != _body.end(); ++it) {
			_cgiState->request_body += it->second;
		}
		_cgiState->bytes_written = 0;
		_cgiState->state = CgiState::WRITING;
	} else {
		close(_cgiState->stdin_fd);
		_cgiState->stdin_fd = -1;
		_cgiState->state = CgiState::READING;
	}
}

void	HttpRequest::executeBin() {
	int		stdin_fd[2];
	int		stdout_fd[2];
	pid_t	pid;

	if (pipe(stdin_fd) < 0 || pipe(stdout_fd) < 0) {
		_status = INTERNAL_ERROR;
		delete _cgiState;
		_cgiState = NULL;
		return ;
	}

	// Set pipes to non-blocking mode
	fcntl(stdin_fd[1], F_SETFL, O_NONBLOCK);  // Parent writes to this
	fcntl(stdout_fd[0], F_SETFL, O_NONBLOCK); // Parent reads from this
	pid = fork();
	if (pid < 0) {
		_status = INTERNAL_ERROR;
		delete _cgiState;
		_cgiState = NULL;
		return ;
	}
	if (pid == 0) {
		childHandler(stdin_fd, stdout_fd);
	}
	else if (pid > 0) {
		parentHandler(stdin_fd, stdout_fd, pid);
	}
}