#include "InetAddress.h"
#include <strings.h>

InetAddress::InetAddress() : address_len(sizeof(address)) {

  bzero(&address, sizeof(address));
}

InetAddress::InetAddress(const char *ip, uint16_t port)
    : address_len(sizeof(address)) {
  bzero(&address, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(ip);
  address.sin_port = htons(port);
  address_len = sizeof(address);
}

InetAddress::~InetAddress() {}
