//
// Created by jonno on 4/17/15.
//

#include "ConnectionData.h"

std::ostream &operator>>(std::ostream &output, AddressDetails &address) {
    switch (address.addressType) {
        case IPV4_ADDRESS:
            //output << Util::hexToString("01");
            break;
        case IPV6_ADDRESS:

            break;
        case DOMAIN_ADDRESS:

            break;
    }
    //input >> address.addressType
    //input >> D.feet >> D.inches;
    return output;
}
