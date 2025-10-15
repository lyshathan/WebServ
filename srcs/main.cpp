#include "config/Config.hpp"
#include "webserv/Webserv.hpp"
#include "parsing/Client.hpp"
#include "ProjectTools.hpp"

#include <signal.h>

volatile sig_atomic_t g_running;

void sigint_handler(int sig)
{
	(void)sig;
	g_running = 0;
	std::cerr << "\n";
	printLog(BLUE, "INFO", "Shutting down gracefully...");
}

int main(int arc, char **arv)
{

	g_running = 1;
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN);


	if (arc != 2)
	{
		if (arc > 2)
			std::cerr << "Too many arguments" << std::endl;
		else if (arc < 2)
			std::cerr << "Missing argument" << std::endl;
		return (0);
	}
	std::string filename = arv[1];
	int checkValid = isValidFile(filename);
	if (checkValid != VALID)
	{
		if (checkValid == NO_EXIST)
			std::cerr << "Configuration file does not exist. Please enter a valid configuration file." << std::endl;
		else if (checkValid == NO_PERMISSION)
			std::cerr << "Permission denied. Please enter a valid configuration file." << std::endl;
		return (1);
	}
	if (filename.length() < 5 || filename.substr(filename.length() - 5) != ".conf")
	{
		std::cerr << "Wrong extension. Please enter a valid configuration file." << std::endl;
		return (1);
	}


	Config config;
	try {
		config = Config(filename);
		printLog(BLUE, "INFO", "Initializing Servers...");
		Webserv webserv(config);
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
