#include "Client.hpp"

int	main(int ac, char **av) {
	if (ac == 2) {
		std::string request = av[1];
		Client a(5);

		a.httpReq->handleRequest(request);

		std::map<std::string, std::string>::iterator it = a.httpReq->getHeaders().begin();

		for (; it != a.httpReq->getHeaders().end(); ++it) {
			std::cout << it->first << " : " << it->second << "\n";
		}

		std::cout << a.httpReq->getMethod()
		<< "\n" << a.httpReq->getUri()
		<< "\n" << a.httpReq->getVersion() << "\n";
	}

	return 0;
}