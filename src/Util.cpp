/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2017. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Util.h"
#include "narf/format.h"
#include <map>

namespace ghLib {

std::string Format(const std::string fmt_str, ...) {
	va_list argp;
	va_start(argp, fmt_str);
	auto ret = narf::util::format(fmt_str, argp);
	va_end(argp);
	return ret;
}

std::string FilterASCII(const std::string in) {
	std::string out;
	for (const char& c : in) {
		if (c < ' ' || c >= 127) {
			out += ".";
		} else {
			out += c;
		}
	}
	return out;
}

std::string EscapeString(const std::string in) {
	std::string out;
	std::map<char, std::string> escapeMap = {
		{'"', "\\\""},
		{'\\', "\\\\"},
		{'\b', "\\b"},
		{'\f', "\\f"},
		{'\n', "\\n"},
		{'\r', "\\r"},
		{'\t', "\\t"}
	};

	for (const char& c : in) {
		auto esc = escapeMap.find(c);
		if (esc == escapeMap.end()) {
			if (c < ' ') {
				out += Format("\\u00%02x,", (unsigned char)c);
			} else {
				out += c;
			}
		} else {
			out += esc->second;
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
