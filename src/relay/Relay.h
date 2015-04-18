//
// Created by jonno on 4/17/15.
//

#ifndef COLD_POTATO_PROXY_RELAY_H
#define COLD_POTATO_PROXY_RELAY_H


#include <ListenServer.h>

class Relay : public ListenServer{
public:
    Relay(int port) : ListenServer(port) {};

    virtual void processConnection(ConnectionData *data) override;
};


#endif //COLD_POTATO_PROXY_RELAY_H
