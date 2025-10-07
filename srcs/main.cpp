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
	std::cout << "\n";
	printLog(BLUE, "INFO", "Shutting down gracefully...");
}

int main(void)
{
	g_running = 1;
	signal(SIGINT, sigint_handler);
	signal(SIGPIPE, SIG_IGN);
	Config config;
	try {
		config = Config("simple.conf");
		printLog(BLUE, "INFO", "Initializing Servers...");
		Webserv webserv(config);
	} catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
