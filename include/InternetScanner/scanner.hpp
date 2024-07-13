#include "InternetScanner/ipv4.hpp"

#include <atomic>

void scanAddresses(AtomicIPv4Address &currentAddress, const IPv4Address stopAddress, const IPv4Address printProgressAddress, const uint32_t timeoutMilliseconds, char *outputBuffer, std::atomic<bool> &running);
