/*
 * Connection.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#include <sstream>
#include "RelayConnection.h"
#include "Constants.h"
#include "Util.h"

#include <iostream>

using namespace std;

RelayConnection::RelayConnection(ConnectionData* connection) {
	mConnectionData = connection;
}

void RelayConnection::handleConnection() {
	int sock = mConnectionData->socket;

	mSock = std::make_unique<Socket>(sock);

	RequestDetails request;
	if (!this->handleRequest(request)) {
		return;
	}

	// Try to connect.
	auto outSock = setupForwardConnection(request);
	if (outSock == nullptr) {
		return;
	}

	this->relayTraffic(outSock);

}

bool RelayConnection::verifyVersion(bytes greeting) {
	if (greeting.size() >= 1) {
		return verifyVersion(greeting[0]);
	}
	return false;
}

bool RelayConnection::verifyVersion(char version) {
	if (version != Constants::Relay::Version::V1) {
		cerr << "Invalid relay version." << endl;
		return false;
	}
	return true;
}

bool RelayConnection::checkAuthentication(char methodCount) {

	bytes methods;
	if (!mSock->receive(methods, methodCount))
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
		mSock->send(Constants::Messages::Auth::InvalidAuth);
		return false;
	}

	//	cerr << "Using NoAuth" << endl;
	mSock->send(Constants::Messages::Auth::UseNoAuth);

	return true;
}

bool RelayConnection::handleRequest(RequestDetails& request) {
	bytes header;

	if (!mSock->receive(header, 3))
	{
		return false;
	}

	if (header.size() != 3)
	{
		return false;
	}

	if (!this->verifyVersion(header[0])) {
		return false;
	}

	// We only support TCP CONNECT, so fail other commands.
	if (header[1] != Constants::Relay::Command::TCPConnection)
	{
		cerr << "Unsupported command: " << hex << header[1] << endl;
		// use this namespace for easier to read messages.
		using namespace Constants::Messages::Request;
		// TODO: Process error messages better.
		// mSock->send(InvalidConnection +  Blank + InvalidDestinationInformation);

		return false;
	}

	return this->readAddressInformation(request);
}

std::shared_ptr<Socket> RelayConnection::setupForwardConnection(const RequestDetails& request) {
	bool connected = false;

	auto outSock = std::make_shared<Socket>();

	switch (request.addressType) {
		case IPV4_ADDRESS:
			connected = outSock->connect4(request.address, request.port);
			break;
		case IPV6_ADDRESS:
			connected = outSock->connect6(request.address, request.port);
			break;
		case DOMAIN_ADDRESS:
			connected = outSock->connect(request.address, request.port);
			break;
		default:
			cerr << "No connection type specified." << endl;
			// send some error message

			break;
	}

	// Send reply.

	// This is wrong - the address & port should be the local address of outSock on the server.
	// Not sure why the client would need this, so I'm just going for 0s.
	if (connected)
	{
		cerr << "Connected!" << endl;
		mSock->send(Util::hexToString("050000") + Util::hexToString("01cb007101abab"));
	}
	else
	{
		cerr << "ProxyConnection failed." << endl;
		mSock->send(Util::hexToString("050400") + Util::hexToString("01cb007101abab")); // Host unreachable.
		return nullptr;
	}
	return outSock;
}

void RelayConnection::relayTraffic(std::shared_ptr<Socket> outSock) {
	pollfd polls[2];
	polls[0].fd = mSock->descriptor();
	polls[0].events = POLLIN; // Listen for data availability.
	polls[0].revents = 0;
	polls[1].fd = outSock->descriptor(); // Will be the output socket.
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
			if (!mSock->receive(data))
			{
//				cerr << "Read error: " << strerror(errno) << endl;
				break;
			}
			if (data.empty())
			{
//				cerr << "Read EOF." << endl;
				break;
			}

			if (!outSock->send(data))
			{
//				cerr << "Write error: " << strerror(errno) << endl;
			}
		}

		if (polls[1].revents & POLLIN)
		{
			bytes data;
			if (!outSock->receive(data))
			{
//				cerr << "Read error: " << strerror(errno) << endl;
				break;
			}
			if (data.empty())
			{
//				cerr << "Read EOF." << endl;
				break;
			}

			if (!mSock->send(data))
			{
//				cerr << "Write error: " << strerror(errno) << endl;
			}
		}

		if ((polls[0].revents & (POLLHUP | POLLNVAL | POLLERR)) || (polls[1].revents & (POLLHUP | POLLNVAL | POLLERR)) )
		{
//			cerr << "ProxyConnection finished." << endl; // Could be an error...
			break;
		}
	}
}

RelayConnection::~RelayConnection() {
	//delete mConnectionData;
}
