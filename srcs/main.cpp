#include "config/parser/Config.hpp"
#include "webserv/Webserv.hpp"
#include "parsing/Client.hpp"

int main(void)
{
	Config config;
	try
	{
		config = Config("simple.conf");
		config.PrintConfig();
		// config.printTokens();
		Webserv webserv(config);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return (0);
}
