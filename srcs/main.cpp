#include "config/Config.hpp"
#include "server/Server.hpp"
#include "parsing/Client.hpp"

int main(void)
{
	Config config("simple.conf");
	config.PrintTokens();
	// Server server();
	return (0);
}