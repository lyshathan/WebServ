#include "Client.hpp"

int	main(int ac, char **av) {
	if (ac == 2) {
		std::string request = av[1];
		Client a(request);

		std::cout << a.httpReq->getMethod()
		<< "\n" << a.httpReq->getUri()
		<< "\n" << a.httpReq->getVersion() << "\n";
	}

	return 0;
}