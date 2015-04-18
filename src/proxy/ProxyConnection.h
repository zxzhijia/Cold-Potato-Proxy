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
#include <Connection.h>
#include "../ConnectionData.h"

class ProxyConnection : public Connection {
private:

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
	bool handleRequest(AddressDetails & request);

	std::shared_ptr<Socket> setupForwardConnection(const AddressDetails & request);
public:
	ProxyConnection(ConnectionData* connection);

	void handleConnection();


	virtual ~ProxyConnection();
};


#endif /* SRC_CONNECTION_H_ */
