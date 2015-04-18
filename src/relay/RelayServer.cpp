//
// Created by jonno on 4/17/15.
//

#include <iostream>
#include <proxy/ProxyConnection.h>
#include "RelayServer.h"

using namespace std;

void RelayServer::processConnection(ConnectionData *data) {

    if (!data)
    {
        // Invalid data.
        cerr << "NULL thread data." << endl;
        return;
    }

    ProxyConnection connection(data);
    connection.handleConnection();
}
