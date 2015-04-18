/*
 * Connection.h
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#ifndef SRC_RELAYCONNECTION_H_
#define SRC_RELAYCONNECTION_H_

#include "Socket.h"
#include "unistd.h"
#include <thread>
#include <memory.h>
#include <Connection.h>
#include "../ConnectionData.h"

class RelayConnection : public  Connection {
private:

	/**
	 * Verifies that the version that the client is requesting is allowed by our proxy server.
	 */
	bool verifyVersion(char version);

	/**
	 * Handles a client's connection request.
	 */
	bool handleRequest(RequestDetails& request);

	std::shared_ptr<Socket> setupForwardConnection(const RequestDetails& request);

	void relayTraffic(std::shared_ptr<Socket> outSock);

public:
	RelayConnection(ConnectionData* connection);

	void handleConnection();


	virtual ~RelayConnection();
};


#endif /* SRC_RELAYCONNECTION_H_ */
