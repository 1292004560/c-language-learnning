#pragma once
#include <arpa/inet.h>

class InetAddress {

public:
  struct sockaddr_in address;
  socklen_t address_length;

  InetAddress();
  InetAddress(const char *ip, uint16_t port);

  ~InetAddress();
};
