#include "config/parser/Config.hpp"
#include "server/Server.hpp"
#include "parsing/Client.hpp"

int main(void)
{
	Config config;
	try
	{
		config = Config("simple.conf");
		config.PrintConfig();
		// config.PrintTokens();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	// Server server(config);
	return (0);
}
