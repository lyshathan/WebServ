#include "ProjectTools.hpp"

uint32_t	fromIPToIntHost(const std::string &IPstr)
{
	std::istringstream iss( IPstr );

	uint32_t IPint = 0;

	for( uint32_t i = 0; i < 4; ++i ) {
		uint32_t part;
		iss >> part;
		if ( iss.fail() || part > 255 ) {
			throw std::runtime_error( "[ERROR] Invalid IP address - Expected [0, 255]" );
		}

		// Shifts the octet to its correct position in the 32-bit integer (host byte order)
		IPint |= part << ( 8 * ( 3 - i ) );

		// check for delimiter except on last iteration
		if ( i != 3 ) {
			char delimiter;
			iss >> delimiter;
			if ( iss.fail() || delimiter != '.' ) {
				throw std::runtime_error( "[ERROR] Invalid IP address - Expected '.' delimiter" );
			}
		}
	}

	// Check for extra element after the 4 octets
	std::string remaining;
	iss >> remaining;
	if (!remaining.empty()) {
		throw std::runtime_error( "[ERROR] Invalid IP address - Extra content after IP" );
	}
	return IPint;
}

uint32_t	fromIPToIntNetwork(const std::string &IPstr)
{
    // Convert from host byte order to network byte order
	return (htonl(fromIPToIntHost(IPstr)));
}

std::string	getCurrentTimeLocal() {
	time_t rawtime;
	struct tm * timeinfo;
	char buffer [80];

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	strftime (buffer,80,"%Y-%m-%d %H:%M:%S", timeinfo);
	return std::string(buffer);
}

void printLog(std::string color, std::string type, std::string str) {
	std::cerr << color << "[" << getCurrentTimeLocal() << "] "
	<< "[" << type << "] " << str << RESET << std::endl;
}
