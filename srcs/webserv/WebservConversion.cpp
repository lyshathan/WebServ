#include "Webserv.hpp"

// #include <arpa/inet.h>

// uint32_t	fromIPToInt(const std::string &IPstr)
// {
// 	// std::string test = "127.0.0.1";
//     std::istringstream iss( IPstr );
    
//     uint32_t IPint = 0;
    
//     for( uint32_t i = 0; i < 4; ++i ) {
//         uint32_t part;
//         iss >> part;
//         if ( iss.fail() || part > 255 ) {
//             throw std::runtime_error( "Invalid IP address - Expected [0, 255]" );
//         }
        
// 		// Shifts the octet to its correct position in the 32-bit integer (host byte order)
//         IPint |= part << ( 8 * ( 3 - i ) );

//         // check for delimiter except on last iteration
//         if ( i != 3 ) {
//             char delimiter;
//             iss >> delimiter;
//             if ( iss.fail() || delimiter != '.' ) {
//                 throw std::runtime_error( "Invalid IP address - Expected '.' delimiter" );
//             }
//         }
//     }
    
//     // Convert from host byte order to network byte order
//     uint32_t networkOrder = htonl(IPint);
    
//     return networkOrder;
// }



// uint32_t	Webserv::fromHostToAddress(std::string hostname)
// {
// 	struct addrinfo		hints;
// 	struct addrinfo		*result;
// 	uint32_t			finalAddress = 0;

// 	// Initialize hints
// 	std::memset(&hints, 0, sizeof(hints));
// 	hints.ai_family = AF_INET;			// IPv4
// 	hints.ai_socktype = SOCK_STREAM;	// TCP socket

// 	int status = getaddrinfo(hostname.c_str(), NULL, &hints, &result);
// 	if (status != 0)
// 	{
// 		std::cerr << "[server] ERROR : getaddrinfo error ( " << strerror(errno) << " )" << RESET << std::endl;
// 		return (0);
// 	}

// 	// Loop through results nested list until first IPv4
// 	struct addrinfo		*ptr;
// 	for (ptr = result ; ptr != NULL ; ptr=ptr->ai_next)
// 	{
// 		if (ptr->ai_family == AF_INET)
// 		{
// 			struct sockaddr_in* validSocketAddr = (struct sockaddr_in*)ptr->ai_addr;
// 			finalAddress = ntohl(validSocketAddr->sin_addr.s_addr);
// 			break;
// 		}
// 	}
// 	freeaddrinfo(result);
// 	return(finalAddress);
// }