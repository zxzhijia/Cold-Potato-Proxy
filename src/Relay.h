//
// Created by jonno on 4/17/15.
//

#ifndef COLD_POTATO_PROXY_RELAY_H
#define COLD_POTATO_PROXY_RELAY_H


class Relay {
private:
    int mPort;
    int mListenFD;

public:
    Relay(int port);
    void Listen();
};


#endif //COLD_POTATO_PROXY_RELAY_H
