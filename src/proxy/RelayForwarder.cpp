//
// Created by jonno on 4/18/15.
//

#include "RelayForwarder.h"

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

bool RelayForwarder::sendRequest(const AddressDetails &dest) {
    // send request information
    // read response
    // figure out what to do based on response

    // we know we are connected
    // v1 tcp_connection address
    mSock->send(Util::hexToString("0101") + "");

    return true;
}
