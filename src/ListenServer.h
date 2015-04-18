//
// Created by jonno on 4/17/15.
//

#ifndef COLD_POTATO_PROXY_LISTENSERVER_H
#define COLD_POTATO_PROXY_LISTENSERVER_H


#include "ConnectionData.h"

class ListenServer {
private:
    int mPort;
    int mListenFD;

public:
    ListenServer(int port);

    virtual void processConnection(ConnectionData *data) = 0;

    virtual void Listen();
};


#endif //COLD_POTATO_PROXY_LISTENSERVER_H
