//
// Created by jonno on 4/17/15.
//

#include "ConnectionData.h"
#include "Util.h"
#include <sstream>
#include <iomanip>
#include "Socket.h"

using namespace std;

std::ostream &operator<<(std::ostream &output, AddressDetails &address) {
    switch (address.addressType) {
        case IPV4_ADDRESS:
            output << Util::hexToString("01");
            break;
        case IPV6_ADDRESS:
            output << Util::hexToString("04");
            break;
        case DOMAIN_ADDRESS:
            output << Util::hexToString("03");
            stringstream ss;
            ss << setfill('0') << setw(2) << hex  << address.address.size();
            output << Util::hexToString(ss.str());
            break;
    }
    output << address.address;

    unsigned char h = (unsigned char)(address.port >> 8);
    unsigned char l = (unsigned char)(address.port - ( h >> 8 ));

    output << h << l;
    return output;
}
