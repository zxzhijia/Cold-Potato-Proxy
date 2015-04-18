//
// Created by jonno on 4/18/15.
//

#include <iostream>
#include <string.h>
#include "Connection.h"
#include "Constants.h"

using namespace std;

bool Connection::readAddressInformation(RequestDetails &rq) {
    bytes address;

    AddressType addressType;
    bytes type;

    // read the type of address


    if (!mSock->receive(type, 1)) {
        return false;
    }

    // Get the type of address they want to connect to.
    switch (type[0])
    {
        case Constants::IP::Type::IPV4: {
            addressType = IPV4_ADDRESS;
            mSock->receive(address, Constants::IP::Length::IPV4);
        }
            break;

        case Constants::IP::Type::Domain: {
            addressType = DOMAIN_ADDRESS;
            bytes len;
            // get length of domain name
            mSock->receive(len, 1);
            // get domain name
            mSock->receive(address, len[0]);
        }
            break;

        case Constants::IP::Type::IPV6: {
            addressType = IPV6_ADDRESS;
            mSock->receive(address, Constants::IP::Length::IPV6);
        }
            break;

        default:
            cerr << "Invalid address type: " << hex << type << endl;
            {
                //using namespace Constants::Messages::Request;
                //mSock->send(InvalidAddressType + Blank + InvalidDestinationInformation);
            }
            return false;
    }

    // Get the port.
    bytes rawPort;
    if (!mSock->receive(rawPort, 2)) {
        cerr << "Could not read the port" << endl;
        return false;
    }
    // horrible port decoding. oh well.
    unsigned char h = (unsigned char)rawPort[0];
    unsigned char l = (unsigned char)rawPort[1];
    int port = (h << 8) + l;

    rq.address = address;
    rq.addressType = addressType;
    rq.port = port;
}

void Connection::relayTraffic(std::shared_ptr<Socket> outSock) {
    pollfd polls[2];
    polls[0].fd = mSock->descriptor();
    polls[0].events = POLLIN; // Listen for data availability.
    polls[0].revents = 0;
    polls[1].fd = outSock->descriptor(); // Will be the output socket.
    polls[1].events = POLLIN;
    polls[1].revents = 0;

    while (true)
    {
        int ret = poll(polls, 2, 60000);
        if (ret == -1)
        {
            cerr << strerror(errno) << endl;
            break;
        }
        if (ret == 0)
        {
//			cerr << "No data (timeout)" << endl;
            break;
        }

        if (polls[0].revents & POLLIN)
        {
            bytes data;
            if (!mSock->receive(data))
            {
//				cerr << "Read error: " << strerror(errno) << endl;
                break;
            }
            if (data.empty())
            {
//				cerr << "Read EOF." << endl;
                break;
            }

            if (!outSock->send(data))
            {
//				cerr << "Write error: " << strerror(errno) << endl;
            }
        }

        if (polls[1].revents & POLLIN)
        {
            bytes data;
            if (!outSock->receive(data))
            {
//				cerr << "Read error: " << strerror(errno) << endl;
                break;
            }
            if (data.empty())
            {
//				cerr << "Read EOF." << endl;
                break;
            }

            if (!mSock->send(data))
            {
//				cerr << "Write error: " << strerror(errno) << endl;
            }
        }

        if ((polls[0].revents & (POLLHUP | POLLNVAL | POLLERR)) || (polls[1].revents & (POLLHUP | POLLNVAL | POLLERR)) )
        {
//			cerr << "Connection finished." << endl; // Could be an error...
            break;
        }
    }
}

Connection::Connection(ConnectionData *connection) {
    mConnectionData = connection;
}

Connection::~Connection() {
    // TODO: stuff
}
