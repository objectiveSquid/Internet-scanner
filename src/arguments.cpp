#include "InternetScanner/arguments.hpp"
#include "InternetScanner/ipv4.hpp"
#include "InternetScanner/math.hpp"

#include <arpa/inet.h>

IPv4Address convertIPv4Address(const std::string &addressAsString) {
    in_addr outputAddress;
    int isValid = inet_aton(addressAsString.c_str(), &outputAddress);
    if (!isValid)
        throw std::invalid_argument("invalid ipv4 address");
    return ntohl(outputAddress.s_addr);
}

void initializeArgumentParser(argparse::ArgumentParser &argumentParser) {
    argumentParser.add_argument("-o", "--output")
        .help("The output file")
        .required();

    argumentParser.add_argument("--timeout")
        .help("Connection timeout in milliseconds")
        .scan<'i', uint32_t>();

    argumentParser.add_argument("--threads")
        .help("How many threads to use")
        .scan<'i', uint16_t>();

    argumentParser.add_argument("--start-ipv4")
        .help("On which IPv4 address to start")
        .action([](const std::string &addressAsString) -> IPv4Address {
            return roundToPrevious8(convertIPv4Address(addressAsString));
        });

    argumentParser.add_argument("--stop-ipv4")
        .help("On which IPv4 address to stop")
        .action([](const std::string &addressAsString) -> IPv4Address {
            return roundToNext8(convertIPv4Address(addressAsString));
        });

    argumentParser.add_argument("--print-progress-ipv4")
        .help("On which IPv4 address increments to print the progress")
        .action([](const std::string &addressAsString) -> IPv4Address {
            return convertIPv4Address(addressAsString);
        });

    argumentParser.add_argument("--save-progress-ipv4")
        .help("On which IPv4 address increments to save the progress")
        .action([](const std::string &addressAsString) -> IPv4Address {
            return convertIPv4Address(addressAsString);
        });
}
