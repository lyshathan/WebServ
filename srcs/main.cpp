#include "config/Config.hpp"
#include "webserv/Webserv.hpp"
#include "parsing/Client.hpp"

#include <signal.h>

volatile sig_atomic_t g_running;

void sigint_handler(int sig)
{
	(void)sig;
	g_running = 0;
	std::cout << "\n[Server] Shutting down gracefully..." << std::endl;
}

int main(void)
{
	g_running = 1;
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN); // Ignore SIGPIPE to handle broken pipes gracefully
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
