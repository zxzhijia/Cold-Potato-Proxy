/*
 * ProxyServer.h
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#ifndef SRC_PROXYSERVER_H_
#define SRC_PROXYSERVER_H_

#include "Connection.h"


class ProxyServer {
private:
	int mPort;
	int mListenFD;
public:
	ProxyServer(int port);
	virtual ~ProxyServer();

	void Listen();
};

#endif /* SRC_PROXYSERVER_H_ */
