//
// Created by jonno on 4/17/15.
//

#ifndef COLD_POTATO_PROXY_CONNECTIONDATA_H
#define COLD_POTATO_PROXY_CONNECTIONDATA_H

#include <string>
#include <netinet/in.h>

enum AddressType {
    IPV4_ADDRESS,
    IPV6_ADDRESS,
    DOMAIN_ADDRESS,
    INVALID
};

struct ConnectionData
{
    int socket; // The socket.
    sockaddr_in client; // The address of the client.
};

struct RequestDetails
{
    AddressType addressType;
    std::string address;
    uint16_t port;
};


#endif //COLD_POTATO_PROXY_CONNECTIONDATA_H
