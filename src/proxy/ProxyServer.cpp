/*
 * ProxyServer.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#include "ProxyServer.h"
#include "Socket.h"
#include <thread>
#include "string.h"
#include "Address.h"
#include "Common.h"
#include "unistd.h"
#include <iostream>
#include "ProxyConnection.h"

using namespace std;

void ProxyServer::processConnection(ConnectionData *data) {
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
