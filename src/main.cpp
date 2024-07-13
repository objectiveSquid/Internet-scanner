#include "InternetScanner/arguments.hpp"
#include "InternetScanner/scanner.hpp"

#include <argparse.hpp>
#include <atomic>
#include <csignal>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <netinet/in.h>
#include <stdexcept>
#include <thread>

#define ARGUMENT_ERROR_RETURN_CODE 2

#define CHUNK_SIZE 128

std::atomic<bool> running = true;

int main(int argc, char **argv) {
    argparse::ArgumentParser argumentParser("InternetScanner");
    initializeArgumentParser(argumentParser);

    uint16_t threadCount = 50;
    uint32_t timeoutMilliseconds = 250;
    IPv4Address startAddress = 0x00000000; // 0.0.0.0
    std::atomic<IPv4Address> currentAddress(startAddress);
    IPv4Address stopAddress = 0xFFFFFFFF;    // 255.255.255.255
    IPv4Address printProgressAddress = 256;  // 0.0.1.0
    IPv4Address saveProgressAddress = 65536; // 0.1.0.0
    std::string outputFilename;

    try {
        argumentParser.parse_args(argc, argv);

        outputFilename = argumentParser.get<std::string>("--output");

        if (argumentParser.is_used("--threads"))
            threadCount = argumentParser.get<uint16_t>("--threads");

        if (argumentParser.is_used("--timeout"))
            timeoutMilliseconds = argumentParser.get<uint32_t>("--timeout");

        if (argumentParser.is_used("--start-ipv4"))
            currentAddress = startAddress = argumentParser.get<IPv4Address>("--start-ipv4");

        if (argumentParser.is_used("--stop-ipv4"))
            stopAddress = argumentParser.get<IPv4Address>("--stop-ipv4");

        if (argumentParser.is_used("--print-progress-ipv4"))
            printProgressAddress = argumentParser.get<IPv4Address>("--print-progress-ipv4");

        if (argumentParser.is_used("--save-progress-ipv4"))
            saveProgressAddress = argumentParser.get<IPv4Address>("--save-progress-ipv4");
    } catch (const std::runtime_error &error) {
        std::cerr << error.what() << '\n';
        std::cerr << argumentParser;
        return ARGUMENT_ERROR_RETURN_CODE;
    }

    // we do not use the stack because the full address range is ~500mb
    char *outputBuffer = new char[(stopAddress - startAddress) / 8];

    std::vector<std::thread> scannerThreads;
    scannerThreads.reserve(threadCount);

    for (uint16_t threadIndex = 0; threadIndex < threadCount; ++threadIndex)
        scannerThreads.emplace_back(
            scanAddresses,
            std::ref(currentAddress),
            stopAddress,
            printProgressAddress,
            timeoutMilliseconds,
            outputBuffer,
            std::ref(running));

    for (auto &thread : scannerThreads)
        thread.join();

    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile.is_open()) {
        std::cerr << "Failed to open output file.\n";
        return EXIT_FAILURE;
    }

    // seeking out of the file bounds just fills the skipped section with zeroes
    outputFile.seekp(startAddress / 8);
    std::streamsize writeCount = (currentAddress - startAddress) / 8;
    if ((currentAddress - startAddress) % 8)
        writeCount += 1;
    outputFile.write(outputBuffer, writeCount);
    outputFile.close();

    delete[] outputBuffer;

    return EXIT_SUCCESS;
}
