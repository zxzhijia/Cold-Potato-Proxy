//
// Created by jonno on 4/17/15.
//

#include <iostream>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <proxy/ProxyConnection.h>
#include "Relay.h"
#include "ConnectionData.h"

using namespace std;

void Relay::processConnection(ConnectionData *data) {
    // The socket.
    ConnectionData* pDat = data;

    if (!pDat)
    {
        // Invalid data.
        cerr << "NULL thread data." << endl;
        return;
    }

    ProxyConnection connection(data);
    connection.handleConnection();
}
