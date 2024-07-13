#include "InternetScanner/ipv4.hpp"

#include <sstream>
#include <string>

std::string uint32ToIpString(IPv4Address ipAddress) {
    std::ostringstream outputString;
    outputString << ((ipAddress >> 24) & 0xFF) << '.'
                 << ((ipAddress >> 16) & 0xFF) << '.'
                 << ((ipAddress >> 8) & 0xFF) << '.'
                 << (ipAddress & 0xFF);
    return outputString.str();
}
