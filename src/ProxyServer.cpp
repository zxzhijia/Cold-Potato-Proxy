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

using namespace std;

struct ThreadData
{
	int socket; // The socket.
	sockaddr_in client; // The address of the client.
};

void* Connection(ThreadData* data)
{
//	cerr << "Thread created" << endl;

	// The socket.
	ThreadData* pDat = data;

	if (!pDat)
	{
		// Invalid data.
		cerr << "NULL thread data." << endl;
		return NULL;
	}

	// Delete the temporary data. PS: Java is shit.
	int sock = pDat->socket;
	sockaddr_in client = pDat->client;
	delete pDat;

	// This socket object will close the connection automatically when it goes out of scope.
	Socket inSock(sock);

	// Read 2 byte version code.
	bytes ver;
	if (!inSock.receive(ver, 2))
	{
		cerr << "Couldn't receive 2 byte SOCKS version." << endl;
		return NULL;
	}

	// Only support SOCKS 5.
	if (ver.size() != 2 || ver[0] != 0x05)
	{
		cerr << "Invalid request." << endl;
		return NULL;
	}

//	cerr << "Version OK." << endl;

	// Receive the authentication methods the client supports.
	// They basically only support the no authentication method. Rubbish clients.
	bytes methods;
	if (!inSock.receive(methods, ver[1])) // ver[1] is the number of authentication methods.
	{
		return NULL;
	}
	bool hasNoAuth = false;

	for (unsigned int i = 0; i < methods.size(); ++i)
	{
		if (methods[i] == 0x00)
			hasNoAuth = true;
	}

//	cerr << "Got " << methods.size() << " auth methods." << endl;

	if (!hasNoAuth)
	{
		cerr << "Client doesn't support unauthenticated sessions." << endl;
		inSock.send(hexBytes("05FF"));
		return NULL;
	}

//	cerr << "Using NoAuth" << endl;
	inSock.send(hexBytes("0500"));

//	cerr << "Authenticated!" << endl;

	// Ok they are authenticated, now get the thing they are trying to do (we only support CONNECT).

	bytes head;
	if (!inSock.receive(head, 4))
	{
		return NULL;
	}

//	cerr << "Received bytes: " << head.size() << endl;

	if (head.size() != 4 || head[0] != 0x05)
	{
		// Invalid version.
		return NULL;
	}

//	cerr << "Got request method: " << (int)head[1] << endl;

	// We only support TCP CONNECT, so fail other commands.
	if (head[1] != 0x01) // TCP CONNECT
	{
		// Unsupported command.
		cerr << "Unsupported command." << endl;
		inSock.send(hexBytes("050200010000000000"));
		return NULL;
	}

//	cerr << "TCP connect. Address type: " << (int)head[3] << endl;

	bytes address;
	int port;
	enum AddressType
	{
		IPV4_ADDRESS,
		IPV6_ADDRESS,
		DOMAIN_ADDRESS
	} addressType;

	// Get the type of address they want to connect to.
	switch (head[3])
	{
	case 0x01: // IPv4
		addressType = IPV4_ADDRESS;
		inSock.receive(address, 4);
		break;
	case 0x03: // Domain
	{
		addressType = DOMAIN_ADDRESS;
		bytes len;
		inSock.receive(len, 1);
		inSock.receive(address, len[0]);
		cout << "Address: " << address << endl;
	}
		break;
	case 0x04: // IPv6
		addressType = IPV6_ADDRESS;
		inSock.receive(address, 16);
		break;
	default:
		cerr << "Invalid address type." << endl;
		inSock.send(hexBytes("050200010000000000"));
		return NULL;
	}

	// Get the port.
	bytes rawPort;
	inSock.receive(rawPort, 2);
	unsigned char h = rawPort[0];
	unsigned char l = rawPort[1];
	port = (h << 8) + l; // TODO: Check ports with bytes > 128

	//	cerr << "Port: " << port << endl;

	// Try to connect.

	Socket outSock;

	cerr << "Connecting " << endl;
	bool connected = false;
	switch (addressType)
	{
	case IPV4_ADDRESS:
		cerr << "Connecting to ipv4 address: " <<
(int)(unsigned char)address[0] << "." << (int)(unsigned char)address[1]
<< "."
<<
(int)(unsigned char)address[2] << "." << (int)(unsigned char)address[4]
<<
":"
<<
port << endl;
		connected = outSock.connect4(address, port);
		break;
	case IPV6_ADDRESS:
		cerr << "Connecting to ipv6 address." << endl;
		connected = outSock.connect6(address, port);
		break;
	case DOMAIN_ADDRESS:
		cerr << "Connecting to " << address << endl;
		connected = outSock.connect(address, port);
		break;
	default:
		cerr << "Internal error! No connection type." << endl;
		break;
	}

	// Send reply.

	// This is wrong - the address & port should be the local address of outSock on the server.
	// Not sure why the client would need this, so I'm just going for 0s.
	if (connected)
	{
		cerr << "Connected!" << endl;
		inSock.send(hexBytes("050000") + hexBytes("01cb007101abab"));
	}
	else
	{
		cerr << "Connection failed." << endl;
		inSock.send(hexBytes("050400") + hexBytes("01cb007101abab")); // Host unreachable.
		return NULL;
	}

	pollfd polls[2];
	polls[0].fd = inSock.descriptor();
	polls[0].events = POLLIN; // Listen for data availability.
	polls[0].revents = 0;
	polls[1].fd = outSock.descriptor(); // Will be the output socket.
	polls[1].events = POLLIN;
	polls[1].revents = 0;


	while (true)
	{
		int ret = poll(polls, 2, 60000);
		if (ret == -1)
		{
			cerr << strerror(errno) << endl;
			break;
		}
		if (ret == 0)
		{
//			cerr << "No data (timeout)" << endl;
			break;
		}

		if (polls[0].revents & POLLIN)
		{
			bytes data;
			if (!inSock.receive(data))
			{
//				cerr << "Read error: " << strerror(errno) << endl;
				break;
			}
			if (data.empty())
			{
//				cerr << "Read EOF." << endl;
				break;
			}

			if (!outSock.send(data))
			{
//				cerr << "Write error: " << strerror(errno) << endl;
			}
		}

		if (polls[1].revents & POLLIN)
		{
			bytes data;
			if (!outSock.receive(data))
			{
//				cerr << "Read error: " << strerror(errno) << endl;
				break;
			}
			if (data.empty())
			{
//				cerr << "Read EOF." << endl;
				break;
			}

			if (!inSock.send(data))
			{
//				cerr << "Write error: " << strerror(errno) << endl;
			}
		}

		if (polls[0].revents & (POLLHUP | POLLNVAL | POLLERR) || polls[1].revents & (POLLHUP | POLLNVAL | POLLERR))
		{
//			cerr << "Connection finished." << endl; // Could be an error...
			break;
		}
	}


	return NULL;
}


ProxyServer::ProxyServer(int port) {
	mPort = port;
	mListenFD = -1;
}

ProxyServer::~ProxyServer() {
	// TODO Auto-generated destructor stub
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

	if (bind(mListenFD, reinterpret_cast<sockaddr*>(&listen_addr), sizeof(listen_addr)) == -1)
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
		ThreadData* pDat = new ThreadData;
		pDat->socket = newSock;
		pDat->client = client_addr;

		// Create a new thread for the socket!
		thread newConnection;
		newConnection = thread(Connection, pDat);
		newConnection.detach();
	}

	close(mListenFD);
}
