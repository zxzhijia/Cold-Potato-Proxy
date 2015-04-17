/*
 * Constants.h
 *
 *  Created on: Apr 11, 2015
 *      Author: jonno
 */

#ifndef SRC_CONSTANTS_H_
#define SRC_CONSTANTS_H_

#include <string>
#include "Util.h"

namespace Constants {
	namespace SOCKS {
		namespace Version {
			const int V5 = 0x05;
		};
		namespace Authentication {
			const int NoAuth = 0x00;
			const int Invalid = 0xFF;
		};
		namespace Command {
			const int TCPConnection = 0x01;
		};
	};
	namespace IP {
		namespace Type {
			const int IPV4 = 0x01;
			const int Domain = 0x03;
			const int IPV6 = 0x04;
		};
		namespace Length {
			const int IPV4 = 4;
			const int IPV6 = 16;
		};
	};

	namespace Messages {
		namespace Auth {
			const std::string InvalidAuth = Util::hexToString("05FF");
			const std::string UseNoAuth = Util::hexToString("0500");
		};

	}

};

#endif /* SRC_CONSTANTS_H_ */
