#include "CgiHandler.hpp"

/******************************************************************************/
/*							Helpers											  */
/******************************************************************************/

void	CgiHandler::markDone() {
	_cgiStage = CGI_DONE;
	// std::cerr << "[DEBUG] CGI " << _pid << " completed successfully.\n";
}

void	CgiHandler::markError(const std::string &err) {
	(void)err;
	_cgiStage = CGI_ERROR;
	// std::cerr << "[CGI ERROR] " << err << std::endl;
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
		_stdinFd = -1;
	}

	if (_stdoutFd > 0) {
		removeFd.push_back(_stdoutFd);
		// should we keep the close here? CHECK_OUT
		_stdoutFd = -1;
	}

	if (_pid > 0) {
		int status;
		pid_t result = waitpid(_pid, &status, WNOHANG);
		if (result == 0) {
			kill(_pid, SIGKILL);
			waitpid(_pid, &status, 0);
		}
		_pid = -1;
	}

	if (_cgiStage != CGI_ERROR)
		_cgiStage = CGI_DONE;

	// std::cerr << "[CGI CLEANUP] Completed cleanup" << std::endl;
}

