#include "HttpRequest.hpp"

/******************************************************************************/
/*							LOCATION FUNCTIONS								  */
/******************************************************************************/

void HttpRequest::pickLocationConfig() {
	const std::vector<LocationConfig> &locations = _server->getLocations();
	std::vector<LocationConfig>::const_iterator it = locations.begin();
	std::vector<LocationConfig>::const_iterator bestMatch = locations.end();
	size_t longestMatch = 0;

	for (; it != locations.end(); ++it) {
		std::string	path = it->getPath();

		if (_uri.length() >= path.length()) {
			if (it->getExactMatch()) {
				if (path == _uri) {
					bestMatch = it;
					break ;
				}
			}
			else if (_uri.compare(0, path.length(), path) == 0) {
				if (path.length() > longestMatch) {
					longestMatch = path.length();
					bestMatch = it;
				}
			}
		}
	}
	if (bestMatch != locations.end()) {
		_location = &(*bestMatch);
		// std::cout << "Location chosen " << _location->getPath() << "\n";
	} else
		_location = &(*locations.begin());
}