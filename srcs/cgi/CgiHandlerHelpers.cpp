#include "CgiHandler.hpp"

/******************************************************************************/
/*							Helpers											  */
/******************************************************************************/

void	CgiHandler::markDone() {
	_cgiStage = CGI_DONE;
	std::cerr << "[DEBUG] CGI " << _pid << " completed successfully.\n";
}

void	CgiHandler::markError(const std::string &err) {
	_cgiStage = CGI_ERROR;
	std::cerr << "[CGI ERROR] " << err << std::endl;
}

bool	CgiHandler::isFinished() const {
	return (_cgiStage == CGI_DONE || _cgiStage == CGI_ERROR);
}

bool	CgiHandler::hasError() const {
	return (_cgiStage == CGI_ERROR);
}

void	CgiHandler::cleanUp(std::vector<int> &removeFd) {
	if (_stdinFd > 0) {
		removeFd.push_back(_stdinFd);
		close(_stdinFd);
		_stdinFd = -1;
	}

	if (_stdoutFd > 0) {
		removeFd.push_back(_stdoutFd);
		close(_stdoutFd);
		_stdoutFd = -1;
	}

	if (_pid > 0) {
		int status;
		waitpid(_pid, &status, WNOHANG);
		_pid = -1;
	}

	if (_cgiStage != CGI_ERROR)
		_cgiStage = CGI_DONE;

	std::cerr << "[CGI CLEANUP] Completed cleanup for PID " << _pid << std::endl;
}

