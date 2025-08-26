#include "config/parser/Config.hpp"
#include "webserv/Webserv.hpp"
#include "parsing/Client.hpp"

int main(void)
{
	Config config;
	try
	{
		config = Config("simple.conf");
		//config.PrintConfig();
		// config.PrintTokens();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	Webserv webserv(config);
	return (0);
}
