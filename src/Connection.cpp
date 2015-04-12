/*
 * Connection.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#include "Connection.h"
#include "Constants.h"

#include <iostream>

using namespace std;

Connection::Connection(ConnectionData connection) {
	mConnectionData = connection;
}

void Connection::handleConnection() {
	int sock = mConnectionData.socket;
	sockaddr_in client = mConnectionData.client;

	mSock = std::make_shared<Socket>(sock);

	if (!this->receiveGreeting()) {
		return;
	}



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
		return;
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
}

bool Connection::verifyVersion(bytes greeting) {
	if (greeting.size() >= 1) {
		return verifySOCKSVersion(greeting[0]);
	}
	return false;
}

bool Connection::verifySOCKSVersion(char version) {
	if (version != Constants::SOCKS::Version::V5) {
		cerr << "Invalid SOCKS Version." << endl;
		return false;
	}
	return true;
}

bool Connection::checkAuthentication() {
	bytes methods;
	if (!mSock->receive(methods, 1)) {
		return false;
	}

	if (!mSock->receive(methods, methods[1]))
	{
		return false;
	}

	bool allowsNoAuth = false;

	for (unsigned int i = 0; i < methods.size(); ++i)
	{
		if (methods[i] == Constants::SOCKS::Authentication::NoAuth)
			allowsNoAuth = true;
	}

	if (!allowsNoAuth)
	{
		cerr << "Client doesn't support unauthenticated sessions." << endl;
		// socks V5, no auth methods
		mSock->send(hexBytes("05FF"));
		return false;
	}

	//	cerr << "Using NoAuth" << endl;
	mSock->send(hexBytes("0500"));

	return true;
}

bool Connection::receiveGreeting() {

	bytes version;
	if (!mSock->receive(version, 1)) {
		cerr << "Could not read the SOCKS version" << endl;
		return false;
	}

	if (!this->verifyVersion(version)) {
		return false;
	}

	if (!this->checkAuthentication()) {
		return false;
	}

	return true;
}

bool Connection::handleRequest() {
	bytes header;

	if (!mSock->receive(header, 4))
	{
		return false;
	}

	if (header.size() != 4)
	{
		return false;
	}

	this->verifySOCKSVersion(header[0]);

	// We only support TCP CONNECT, so fail other commands.
	if (header[1] != Constants::SOCKS::Command::TCPConnection)
	{
		// Unsupported command.
		cerr << "Unsupported command." << endl;
		mSock->send(hexBytes("050200010000000000"));
		return false;
	}
	// header[2] is 0x00 - reserved

	bytes address;
	AddressType addressType;

	// Get the type of address they want to connect to.
	switch (header[3])
	{
		case Constants::IP::Type::IPV4: {
			addressType = IPV4_ADDRESS;
			mSock->receive(address, Constants::IP::Length::IPV4);
		}
			break;

		case Constants::IP::Type::Domain: {
			addressType = DOMAIN_ADDRESS;
			bytes len;
			// get length of domain name
			mSock->receive(len, 1);
			// get domain name
			mSock->receive(address, len[0]);
		}
			break;

		case Constants::IP::Type::IPV6: {
			addressType = IPV6_ADDRESS;
			mSock->receive(address, Constants::IP::Length::IPV6);
		}
			break;

		default:
			cerr << "Invalid address type." << endl;
			// TODO: Fix this trash
			mSock->send(hexBytes("050200010000000000"));
			return false;
	}

	// Get the port.
	bytes rawPort;
	inSock.receive(rawPort, 2);
	unsigned char h = rawPort[0];
	unsigned char l = rawPort[1];
	port = (h << 8) + l; // TODO: Check ports with bytes > 128

}



Connection::~Connection() {
	// TODO Auto-generated destructor stub
}
