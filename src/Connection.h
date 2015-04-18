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

    bool readAddressType(RequestDetails& rq);

public:


};


#endif //COLD_POTATO_PROXY_CONNECTION_H
