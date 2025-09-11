#include "config/Config.hpp"
#include "webserv/Webserv.hpp"
#include "parsing/Client.hpp"

#include <signal.h>

volatile sig_atomic_t running;

void sigint_handler(int sig)
{
	(void)sig;
	running = 0;
	std::cout << "SIGNAL" << std::endl;
}

int main(void)
{
	running = 1;
	signal(SIGINT, sigint_handler);
	Config config;
	try
	{
		config = Config("simple.conf");
		// config.PrintConfig();
		// config.printTokens();
		Webserv webserv(config);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	std::cout << "End of program" << std::endl;

	return (0);
}
