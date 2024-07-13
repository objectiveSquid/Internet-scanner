#include "InternetScanner/scanner.hpp"
#include "InternetScanner/ipv4.hpp"
#include "InternetScanner/math.hpp"
#include "InternetScanner/ping.hpp"

#include <atomic>
#include <iostream>

void scanAddresses(AtomicIPv4Address &currentAddress, const IPv4Address stopAddress, const IPv4Address printProgressAddress, const uint32_t timeoutMilliseconds, char *outputBuffer, std::atomic<bool> &running) {
    IPv4Address currentlyScannedAddress;

    while ((currentlyScannedAddress = currentAddress++) <= stopAddress && running.load()) {
        // use std::endl cause this time i actually want to flush the buffer
        if ((currentlyScannedAddress % printProgressAddress) == 0 && currentlyScannedAddress != 0x00000000)
            std::cout << "Progress: " << uint32ToIpString(currentlyScannedAddress) << std::endl;

        outputBuffer[currentlyScannedAddress / 8] |= ping(currentlyScannedAddress, timeoutMilliseconds) << (7 - (currentlyScannedAddress % 8));
    }

    running.store(false);
}
