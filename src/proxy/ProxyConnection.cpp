/*
 * ProxyConnection.cpp
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#include <sstream>
#include "ProxyConnection.h"
#include "Constants.h"
#include "RelayForwarder.h"

#include <iostream>

using namespace std;

ProxyConnection::ProxyConnection(ConnectionData* connection) : Connection(connection) {

}

void ProxyConnection::handleConnection() {
	int sock = mConnectionData->socket;
	//sockaddr_in client = mConnectionData->client;

	mSock = std::make_unique<Socket>(sock);

	if (!this->receiveGreeting()) {
		return;
	}

	AddressDetails request;
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

bool ProxyConnection::verifyVersion(bytes greeting) {
	if (greeting.size() >= 1) {
		return verifySOCKSVersion(greeting[0]);
	}
	return false;
}

bool ProxyConnection::verifySOCKSVersion(char version) {
	if (version != Constants::SOCKS::Version::V5) {
		cerr << "Invalid SOCKS Version." << endl;
		return false;
	}
	return true;
}

bool ProxyConnection::checkAuthentication(char methodCount) {

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
		mSock->send(Constants::Messages::SOCKS::Auth::InvalidAuth);
		return false;
	}

	//	cerr << "Using NoAuth" << endl;
	mSock->send(Constants::Messages::SOCKS::Auth::UseNoAuth);

	return true;
}

bool ProxyConnection::receiveGreeting() {

	bytes version;
	if ((!mSock->receive(version, 2)) || (version.size() != 2) ) {
		cerr << "Could not read the SOCKS version" << endl;
		return false;
	}

	if (!this->verifySOCKSVersion(version[0])) {
		cout << hex << version[0] << version[1]<< endl;
		return false;
	}

	// TODO: simplify logic
	if (!this->checkAuthentication(version[1])) {
		return false;
	}

	return true;
}

bool ProxyConnection::handleRequest(AddressDetails & request) {
	bytes header;

	if (!mSock->receive(header, 3))
	{
		return false;
	}

	if (header.size() != 3)
	{
		return false;
	}

	if (!this->verifySOCKSVersion(header[0])) {
		return false;
	}

	// We only support TCP CONNECT, so fail other commands.
	if (header[1] != Constants::SOCKS::Command::TCPConnection)
	{
		cerr << "Unsupported command: " << hex << header[1] << endl;
		// use this namespace for easier to read messages.
		using namespace Constants::Messages::SOCKS::Request;
		mSock->send(InvalidConnection +  Blank + InvalidDestinationInformation);

		return false;
	}

	// header[2] is 0x00 - reserved

	if (!this->readAddressInformation(request)) {
		using namespace Constants::Messages::SOCKS::Request;
		mSock->send(InvalidAddressType + Blank + InvalidDestinationInformation);
		return false;
	}
	return true;
}

std::shared_ptr<Socket> ProxyConnection::setupForwardConnection(const AddressDetails & request) {
	auto outSock = std::make_shared<Socket>();

	// Send reply.
	RelayForwarder rf(request);
	rf.connect();

	// This is wrong - the address & port should be the local address of outSock on the server.
	// Not sure why the client would need this, so I'm just going for 0s.
	if (outSock->connect(request))
	{
		using namespace Constants::Messages::SOCKS::Request;
		cerr << "Connected!" << endl;
		mSock->send(RequestGranted + Blank + Util::hexToString("01cb007101abab"));
	}
	else
	{
		using namespace Constants::Messages::SOCKS::Request;
		cerr << "ProxyConnection failed." << endl;
		mSock->send(HostUnreachable + Blank + Util::hexToString("01cb007101abab")); // Host unreachable.
		return nullptr;
	}
	return outSock;
}

ProxyConnection::~ProxyConnection() {
	//delete mConnectionData;
}
