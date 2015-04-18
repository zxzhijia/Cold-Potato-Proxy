//
// Created by jonno on 4/17/15.
//

#include <iostream>
#include "RelayServer.h"
#include "RelayConnection.h"

using namespace std;

void RelayServer::processConnection(ConnectionData *data) {

    if (!data)
    {
        // Invalid data.
        cerr << "NULL thread data." << endl;
        return;
    }

    RelayConnection connection(data);
    connection.handleConnection();
}
