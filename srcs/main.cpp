#include "config/Config.hpp"
#include "server/Server.hpp"
#include "parsing/Client.hpp"

int main(void)
{
	try
	{
		Config config("example.conf");
		// config.PrintTokens();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	// Server server();
	return (0);
}
