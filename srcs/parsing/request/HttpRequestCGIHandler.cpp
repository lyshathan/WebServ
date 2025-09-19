#include "HttpRequest.hpp"

/******************************************************************************/
/*								CGI HANDLER									  */
/******************************************************************************/

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

void	HttpRequest::checkGGI() {
	std::string cgiPath = _location->getCGIPath();
	std::string cgiExtension = _location->getCGIExtension();

	if (cgiPath.empty() || cgiExtension.empty())
		return;

	size_t pos = _uri.find_last_of(".");
	if (pos != std::string::npos) {
		std::string extension = _uri.substr(pos);
		if (extension.compare(cgiExtension) == 0)
			executeBin(cgiPath);
	}
}