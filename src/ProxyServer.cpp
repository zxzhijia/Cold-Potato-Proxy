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
#include "Connection.h"

using namespace std;

void processConnection(ConnectionData* data)
{
	// The socket.
	ConnectionData* pDat = data;

	if (!pDat)
	{
		// Invalid data.
		cerr << "NULL thread data." << endl;
		return;
	}

	Connection connection(data);
	connection.handleConnection();
}


ProxyServer::ProxyServer(int port) {
	mPort = port;
	mListenFD = -1;
}

ProxyServer::~ProxyServer() {
	// TODO Auto-generated destructor stub
	if (mListenFD != -1) {
		close(mListenFD);
	}
}

void ProxyServer::Listen() {
	if (mListenFD != -1) {
		// close
		return;
	}

	cout << "Starting on port " << mPort << "." << endl;
	mListenFD = socket(PF_INET, SOCK_STREAM, 0);
	if (mListenFD == -1)
	{
		cerr << "Error creating listen socket: " << strerror(errno) << endl;
		return;
	}

	// Try to reuse socket. We don't care if it fails really. Yeah yeah security.
	int optval = 1;
	setsockopt(mListenFD, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	sockaddr_in listen_addr;
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(mPort);
	listen_addr.sin_addr.s_addr = INADDR_ANY;

	if (::bind(mListenFD, reinterpret_cast<sockaddr*>(&listen_addr), sizeof(listen_addr)) == -1)
	{
		cerr << "Error binding listen socket: " << strerror(errno) << endl;
		return;
	}

	if (listen(mListenFD, 1024) == -1)
	{
		cerr << "Error listening on listen socket: " << strerror(errno) << endl;
		return;
	}

	while (true)
	{
		sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int newSock = accept(mListenFD, reinterpret_cast<sockaddr*>(&client_addr), &client_addr_len);
		if (newSock == -1)
		{
			cerr << "Error accepting connection: " << strerror(errno) << endl;

			// Check if ctrl-c was pressed.
			if (errno == EINTR)
				break;
			usleep(100000); // Just in case.
			continue;
		}

//		cerr << "Accepted connection." << endl;
		ConnectionData* pDat = new ConnectionData;
		pDat->socket = newSock;
		pDat->client = client_addr;

		// Create a new thread for the socket!
		thread newConnection;
		newConnection = thread(processConnection, pDat);
		newConnection.detach();
	}

	close(mListenFD);
}
