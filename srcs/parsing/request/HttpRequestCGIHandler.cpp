#include "HttpRequest.hpp"

/******************************************************************************/
/*								CGI HANDLER									  */
/******************************************************************************/

void	HttpRequest::cgiHandler() {
	initEnv();

	std::vector<std::string>::iterator it = _env.begin();
	std::cout << "--Env variables --\n";
	for (; it != _env.end(); ++it)
		std::cout << *it << "\n";

	if (!_status)
		_status = OK;
}

void	HttpRequest::executeBin(std::string path) {
	(void)path;
	int		fd[2];
	pid_t	pid;

	pipe(fd);
	pid = fork();

	const char *argv[] = {"/usr/bin/python3",  "var/www/cgi/hello.py", NULL};
	if (pid == 0)
		execve("/usr/bin/python3", (char* const*)argv, NULL);

}