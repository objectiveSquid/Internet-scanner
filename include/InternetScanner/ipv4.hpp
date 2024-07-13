#include <atomic>
#include <stdint.h>
#include <string>

typedef uint32_t IPv4Address;
typedef std::atomic<IPv4Address> AtomicIPv4Address;

std::string uint32ToIpString(IPv4Address ipAddress);
