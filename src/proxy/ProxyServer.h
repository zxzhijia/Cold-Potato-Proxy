/*
 * ProxyServer.h
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#ifndef SRC_PROXYSERVER_H_
#define SRC_PROXYSERVER_H_

#include "ProxyConnection.h"
#include "ListenServer.h"


class ProxyServer : public ListenServer {
private:
	virtual void processConnection(ConnectionData* data);
public:
	ProxyServer(int port) : ListenServer(port) {};
};

#endif /* SRC_PROXYSERVER_H_ */
