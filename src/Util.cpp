/*
 * Util.cpp
 *
 *  Created on: Apr 17, 2015
 *      Author: jonno
 */

#include "Util.h"

#include <stdexcept>

namespace Util {

static const char* const lookupTable = "0123456789ABCDEF";

std::string hexToString(const std::string& hex) {

	size_t len = hex.length();
	if (len & 1) throw std::invalid_argument("Incorrect Length for hex value. Should not be odd.");

	std::string output;
	output.reserve(len / 2);
	for (size_t i = 0; i < len; i += 2)
	{
		char a = toupper(hex[i]);
		const char* p = std::lower_bound(lookupTable, lookupTable + 16, a);
		if (*p != a) {
			throw std::invalid_argument("not a hex digit");
		}

		char b = toupper(hex[i + 1]);
		const char* q = std::lower_bound(lookupTable, lookupTable + 16, b);
		if (*q != b) {
			throw std::invalid_argument("not a hex digit");
		}

		output.push_back(((p - lookupTable) << 4) | (q - lookupTable));
	}
	return output;
}

std::string stringToHex(const std::string& str) {
	size_t len = str.length();

	std::string output;
	output.reserve(2 * len);
	for (size_t i = 0; i < len; ++i)
	{
		const unsigned char c = str[i];
		output.push_back(lookupTable[c >> 4]);
		output.push_back(lookupTable[c & 15]);
	}
	return output;
}


} /* namespace Util */

