//
// Created by jonno on 4/17/15.
//

#ifndef COLD_POTATO_PROXY_CONNECTIONDATA_H
#define COLD_POTATO_PROXY_CONNECTIONDATA_H

#include <string>
#include <iostream>
#include <netinet/in.h>

enum AddressType {
    IPV4_ADDRESS,
    IPV6_ADDRESS,
    DOMAIN_ADDRESS
};

struct ConnectionData
{
    int socket; // The socket.
    sockaddr_in client; // The address of the client.
};

class AddressDetails
{
public:
    AddressType addressType;
    std::string address;
    uint16_t port;

    friend std::ostream &operator<<( std::ostream  &output, AddressDetails &address );
};


#endif //COLD_POTATO_PROXY_CONNECTIONDATA_H
