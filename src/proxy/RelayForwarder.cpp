//
// Created by jonno on 4/18/15.
//

#include <sstream>
#include "RelayForwarder.h"

using namespace std;

RelayForwarder::RelayForwarder(const AddressDetails &relay, const AddressDetails &finalDestination) {
    mSock = std::make_shared<Socket>();

    mFinalDestination = finalDestination;
    mRelay = relay;
}

bool RelayForwarder::connect() {
    if (!mSock->connect(mRelay)) {
        return false;
    }
    if (!this->sendRequest(mFinalDestination)) {
        return false;
    }
    // do other stuff
    return true;
}

bool RelayForwarder::sendRequest(AddressDetails &dest) {
    // send request information
    // read response
    // figure out what to do based on response

    // we know we are connected
    // v1 tcp_connection address
    stringstream address;
    address << dest;
    string data = address.str();
    mSock->send(Util::hexToString("0101") + data);

    return true;
}
