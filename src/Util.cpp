/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Util.h"

namespace ghLib {

double Deadband(double v, double deadband) {
	// If v is within `deadband` of 0, then just return 0.
	// Else, interpolate |v| from [deadband, 1] into [0, 1] and
	// make it negative if it was that before
	return (std::abs(v) < deadband) ? 0 : (std::signbit(v) ? -1 : 1) * Interpolate(std::abs(v), deadband, 1.0, 0.0, 1.0);
}

// From http://stackoverflow.com/a/8098080
std::string Format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::string str;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while(1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}

std::string FilterASCII(const std::string in) {
	std::string out;
	for (const char& c : in) {
		//out += Format("%d,", (unsigned char)c);
		if (c < ' ' || c >= 127) {
			out += ".";
			//out += Format("\\x%02x", (unsigned char)c);
		} else {
			out += c;
		}
	}
	return out;
}

std::vector<std::string>& Tokenize(const std::string &input, char delimeter, std::vector<std::string>& tokens, char escape /* = '\0' */) {
	std::string buf = "";
	bool escaping = false;
	for (int i = 0; i < (int)input.size(); i++) {
		char c = input[i];
		if (!escaping && c == escape) {
			escaping = true;
		} else if (!escaping && c == delimeter) {
			tokens.push_back(buf);
			buf = "";
		} else {
			if (escaping && (c != escape && c != delimeter)) { // We escaped nothing, so put the escape character back
				buf += escape;
			}
			buf += c;
			escaping = false;
		}
	}
	if (buf.size()) {
		tokens.push_back(buf);
	}
	return tokens;
}

std::vector<std::string> Tokenize(const std::string &input, char delimeter, char escape /* = '\0' */) {
	std::vector<std::string> tokens;
	Tokenize(input, delimeter, tokens, escape);
	return tokens;
}

}
