#include "InetAddress.h"
#include <strings.h>

InetAddress::InetAddress(const char *ip, uint16_t port)
    : address_size(sizeof(addr)) {

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  address_size = sizeof(addr);
}

InetAddress::InetAddress() : address_size(sizeof(addr)) {
  bzero(&addr, sizeof(addr));
}

InetAddress::~InetAddress() {}
