//
// Created by jonno on 4/18/15.
//

#ifndef COLD_POTATO_PROXY_CONNECTION_H
#define COLD_POTATO_PROXY_CONNECTION_H

#include <thread>
#include "ConnectionData.h"
#include "Socket.h"


class Connection {
protected:
    ConnectionData* mConnectionData;
    std::unique_ptr<Socket> mSock;

    /**
     * Reads in the address type, then it reads in the correct amount
     * of data depending on the type of address it is.
     * It then will read in the port information as well.
     * Returns false if any error occurred.
     * Returns true if all happened without problems.
     */
    bool readAddressInformation(AddressDetails &rq);

    void relayTraffic(std::shared_ptr<Socket> outSock);

public:
    Connection(ConnectionData* connection);

    virtual ~Connection();

};


#endif //COLD_POTATO_PROXY_CONNECTION_H
