/*
 * Connection.h
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#ifndef SRC_CONNECTION_H_
#define SRC_CONNECTION_H_

#include "Socket.h"
#include "unistd.h"
#include <thread>
#include <memory.h>

enum AddressType {
	IPV4_ADDRESS,
	IPV6_ADDRESS,
	DOMAIN_ADDRESS,
	INVALID
};

struct ConnectionData
{
	int socket; // The socket.
	sockaddr_in client; // The address of the client.
};

struct RequestDetails
{
	AddressType addressType;
	int requestType;
	std::string address;
	int port;
};



class Connection {
private:
	ConnectionData* mConnectionData;
	std::unique_ptr<Socket> mSock;

	/**
	 * Processes the greeting that a client sends when it connects to a SOCKS server.
	 */
	bool receiveGreeting();

	/**
	 * Verifies that the version that the client is requesting is allowed by our proxy server.
	 */
	bool verifyVersion(bytes version);
	bool verifySOCKSVersion(char version);

	/**
	 * Checks the authentication methods that the client supports and determines whether
	 * we will support the connection.
	 */
	bool checkAuthentication(char methodCount);

	/**
	 * Handles a client's connection request.
	 */
	bool handleRequest(RequestDetails& request);

	std::shared_ptr<Socket> setupForwardConnection(const RequestDetails& request);

	void relayTraffic(std::shared_ptr<Socket> outSock);

public:
	Connection(ConnectionData* connection);

	void handleConnection();


	virtual ~Connection();
};


#endif /* SRC_CONNECTION_H_ */
