/*
 * INI file load/save
 * https://github.com/narfblock/narfblock/blob/master/src/narf/ini.cpp
 *
 * Copyright (c) 2015 Daniel Verkamp, Jessica Creighton
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
 * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#define __STDC_FORMAT_MACROS 1
#include <inttypes.h>

#include "ghLib/ini.h"

#include <errno.h>
#include <cstdlib>

static bool isSpace(char c) {
	return c == ' ' || c == '\t';
}


static bool isNewLine(char c) {
	return c == '\r' || c == '\n';
}

void ghLib::INI::warn(const std::string& s) {
	//ghLib::console->println(s);
}

ghLib::INI::Line::Line(const char* data, size_t size) {
	std::string line(data, size);
	raw = line;
	parse();
}

ghLib::INI::Line::Line(const std::string& line) {
	raw = line;
	parse();
}

ghLib::INI::Line::~Line() { }

std::string ghLib::INI::Line::getKey() {
	return key;
}

std::string ghLib::INI::Line::getValue() {
	return value;
}

std::string ghLib::INI::Line::getRaw() {
	return raw;
}

ghLib::INI::Line::Type ghLib::INI::Line::getType() {
	return lineType;
}

std::string ghLib::INI::Line::setValue(std::string newValue) {
	if (value == newValue) {
		return value;
	}
	std::string oldValue = value;
	value = "";
	for (size_t i = 0; i < newValue.size(); i++) {
		char c = newValue.at(i);
		std::unordered_map<char, std::string> escapes {{'\0', "\\0"}, {'\a', "\\a"}, {'\b', "\\b"},
				{'\t', "\\t"}, {'\r', "\\r"}, {'\n', "\\n"}, {';', "\\;"}, {'"', "\\\""}, {'\\', "\\\\"}};
		if (escapes.count(c) > 0) {
			value += escapes.at(c);
		} else if (c < 32 || (uint8_t)c >= 127) {
			char raw[30];
			snprintf(raw, sizeof(raw), "\\x%02x", (uint8_t)c); // TODO: Unicode? Or at least latin-1?
			value += raw;
		} else if (c == ' ' && (i + 1 == newValue.size())) { // Last character is a space, so we need quotes
			value += " \""; // The last space, plus the quote
			value.insert(value.begin(), '"'); // First space
		} else {
			value += c;
		}
	}
	raw = raw.substr(0, valueStartPos) + value + raw.substr(valueStartPos + valueLength, std::string::npos);
	return oldValue;
}


void ghLib::INI::Line::parse() {
	enum class State {
		BeginLine,
		EndLine,
		Ignore,
		Section,
		Key,
		Equals,
		Value,
		ValueEnd
	};
	State state = State::BeginLine;
	enum class QuoteState { None, Inside, Done };
	QuoteState quoteState = QuoteState::None;
	enum class EscapeState { None, Hex, Other }; // To assist with multicharacter escapes
	EscapeState escapeState = EscapeState::None;
	const char* keyStart = nullptr;
	const char* valueStart = nullptr;
	const char* sectionStart = nullptr;
	const char* chars = raw.c_str();
	size_t size = raw.size();
	error = false; // No parse errors
	const char* valueEnd = nullptr;
	std::string hexEscape;
	size_t i = 0;
	for (; i <= size; i++) {
		char c;
		const char* d;
		if (i == size) {
			c = '\0';
			d = &chars[size - 1];
		} else {
			c = chars[i];
			d = &chars[i];
		}
		switch (state) {
			case State::BeginLine:
				if (c == '[') {
					state = State::Section;
					lineType = Type::Section;
				} else if (isSpace(c)) {
					// eat whitespace
				} else if (isNewLine(c)) {
					// Blank line, no need to go further
					lineType = Type::Other;
				} else if (c == ';') {
					state = State::Ignore;
					lineType = Type::Comment;
				} else {
					state = State::Key;
					keyStart = d;
					lineType = Type::Entry;
				};
				break;
			case State::EndLine:
				if (c == '\0' || isNewLine(c)) {
					// We done
				} else if (isSpace(c)) {
					// skip whitespace
				} else if (c == ';') {
					// Rest of line is a comment
					state = State::Ignore;
				} else {
					warn("junk at end of line");
					error = true;
					c = '\0';
				}
				break;
			case State::Section:
				if (sectionStart == nullptr) {
					sectionStart = d;
				}
				if (c == ']') {
					key = std::string(sectionStart, static_cast<size_t>(d - sectionStart));
					state = State::EndLine;
				}
				break;
			case State::Key:
				if (isSpace(c) || c == '=') {
					if (c == '=') {
						state = State::Value;
					} else {
						state = State::Equals;
					}
					key = std::string(keyStart, static_cast<size_t>(d - keyStart));
					keyStart = nullptr;
				} else if (c == '\0' || isNewLine(c)) {
					warn("key without value");
					error = true;
					state = State::Ignore;
				} else {
					// Accumulate characters into key
					// TODO: Are there invalid key characters?
				}
				break;
			case State::Equals:
				if (isSpace(c)) {
					// Eat whitespace
				} else if (c == '=') {
					state = State::Value;
				} else {
					error = true;
					warn("junk after key");
					state = State::Ignore;
				}
				break;
			case State::Value:
				if (valueStart == nullptr) {
					if (isSpace(c)) {
						// eat whitespace between = and value
					} else {
						valueStart = d;
						valueStartPos = i;
						i--;
					}
				} else if (c == '\0' || isNewLine(c) || (c == ';' && quoteState != QuoteState::Inside && escapeState == EscapeState::None)) {
					state = State::ValueEnd;
					i--;
					continue;
				} else {
					// Accumulate any other chars into value
					if (!isSpace(c)) {
						valueEnd = d; // Store the last non-space character so our valueLength is accurate after trimming
					}
					if (c == '\\' && escapeState == EscapeState::None) {
						escapeState = EscapeState::Other;
					} else if (escapeState == EscapeState::Other) {
						std::unordered_map<char, char> escapes {{'0', '\0'}, {'a', '\a'}, {'b', '\b'}, {'t', '\t'}, {'r', '\r'}, {'n', '\n'}};
						if (escapes.count(c) > 0) {
							value += escapes.at(c);
						} else if (c == 'x') { // Hex escape
							escapeState = EscapeState::Hex;
							hexEscape = "";
						} else { // Catches Quote, Semicolon, and Backslash
							value += c;
						}
						if (escapeState == EscapeState::Other) {
							escapeState = EscapeState::None;
						}
					} else if (escapeState == EscapeState::Hex) {
						if (hexEscape.size() < 2 && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) {
							hexEscape += c;
						} else {
							if (hexEscape.size() > 0) {
								value += (char)std::stoi(hexEscape, nullptr, 16);
							} else { // This should probably error?
								value += "\\x";
							}
							escapeState = EscapeState::None;
							i--; // Redo the previous character if not part of the he
							continue;
						}
					} else if (c == '"') {
						switch (quoteState) {
							case QuoteState::None:
								if (valueStart == d) {
									quoteState = QuoteState::Inside;
								} else {
									error = true;
									value += '"';
								}
								break;
							case QuoteState::Inside:
								quoteState = QuoteState::Done;
								break;
							case QuoteState::Done:
								error = true;
								break;
						}
					} else if (quoteState != QuoteState::Done){
						value += c;
					}
				}
				break;
			case State::ValueEnd:
				if (escapeState != EscapeState::None) {
					if (escapeState == EscapeState::Hex) { // We were at the start
						if (hexEscape.size() > 0) {
							value += (char)std::stoi(hexEscape, nullptr, 16); // Reached the end in the middle of a hex escape
						} else { // Should this error?
							value += "\\x"; // We started a hex escape, but reached the end before we even started
						}
					} else {
						value += '\\'; // We started escaping but got to the end, so treat it as a normal slash
					}
				}
				// Trim trailing whitespace. std::string doesn't have a built in trim for some reason :|
				// From here: http://stackoverflow.com/a/17976541
				if (quoteState == QuoteState::None) {
					// We weren't inside of a quote, so we should trim trailing spaces
					auto trimmedback = std::find_if_not(value.rbegin(), value.rend(), [](char c){return isSpace(c);}).base();
					value = std::string(value.begin(), trimmedback);
				}
				valueLength = (size_t)(valueEnd + 1 - valueStart);
				state = State::EndLine;
				break;
			case State::Ignore:
				// Nothing matters </3
				break;
		}
		if (c == '\0' || isNewLine(c)) {
			break;
		}
	}
	raw.resize(i + 1); // Truncate to just the bits we've read
}

ghLib::INI::File::File() { }

ghLib::INI::File::~File() { }

bool ghLib::INI::File::load(const void* data, size_t size) {
	const char* chars = static_cast<const char*>(data);
	size_t i = 0;
	std::string section;
	while (i < size) {
		ghLib::INI::Line line(chars + i, size - i);
		lines.push_back(line);
		i += line.getRaw().size();
		if (line.getType() == ghLib::INI::Line::Type::Section) {
			section = line.getKey() + ".";
		} else if (line.getType() == ghLib::INI::Line::Type::Entry) {
			setString(section + line.getKey(), line.getValue());
		} else {
			// Comment or blank line or something
		}
	}
	return true;
}

std::string ghLib::INI::File::save() {
	// It ain't pretty

	std::string section;
	std::string output;
	std::vector<std::string> savedKeys;
	// First update lines with new values and keep track of which keys exist
	for (auto &line : lines) {
		if (line.getType() == ghLib::INI::Line::Type::Section) {
			section = line.getKey() + ".";
		} else if (line.getType() == ghLib::INI::Line::Type::Entry) {
			line.setValue(getString(section + line.getKey()));
			savedKeys.push_back(section + line.getKey());
		} else {
			// Comment or blank line or something
		}
	}
	// Iterate over all values and add ones which don't exist as lines
	for (auto &pair : values_) {
		auto key = pair.first;
		if (std::count(savedKeys.begin(), savedKeys.end(), key) == 0) {
			std::string section;
			size_t lastEntry = 0;
			for (size_t i = 0; i < lines.size(); i++) {
				auto line = lines.at(i);
				if (line.getType() == ghLib::INI::Line::Type::Section) {
					if ((section == "" && key.find('.') == std::string::npos) ||
							(section != "" && key.find(section + ".") == 0)) {
						// If we were at global and key is at global
						// or if we got to the end of the section which matches our key
						// then insert and break
						break;
					}
					section = line.getKey();
				}
				if (line.getType() != ghLib::INI::Line::Type::Other) {
					lastEntry = (size_t)(i + 1);
				}
			}
			if (section != "" && key.find(section + ".") == std::string::npos || (section == "" && key.find(".") != std::string::npos)) {
				// TODO: Detect which line endings to use
				section = key.substr(0, key.find("."));
				lines.insert(lines.begin() + (int)lastEntry, ghLib::INI::Line("[" + section + "]\n"));
				lastEntry++;
			}
			auto dotPos = key.find(".");
			if (dotPos == std::string::npos) {
				dotPos = 0;
			} else {
				dotPos++;
			}
			// TODO: Detect indentation?
			ghLib::INI::Line newLine((section == "" ? "" : "\t") + key.substr(dotPos, std::string::npos) + " = foo\n");
			newLine.setValue(pair.second);
			lines.insert(lines.begin() + (int)lastEntry, newLine);
		}
	}
	// Iterate again for the output
	for (auto &line : lines) {
		output += line.getRaw();
	}
	return output;
}

std::string ghLib::INI::File::getString(const std::string& key) const {
	if (has(key)) {
		return values_.at(key);
	}
	// TODO: throw exception if key not found?
	return "";
}


void ghLib::INI::File::setString(const std::string& key, const std::string& value) {
	values_[key] = value;
	update(key);
}


void ghLib::INI::File::update(const std::string& key) {
	if (updateHandler) {
		updateHandler(key);
	}
}


bool ghLib::INI::File::has(const std::string& key) const {
	return values_.count(key) != 0;
}


bool ghLib::INI::File::getBool(const std::string& key) const {
	auto raw = getString(key);
	switch (raw[0]) {
	case 'y': // yes
	case 'Y':
	case 't': // true
	case 'T':
	case 'e': // enabled
	case 'E':
	case '1':
		return true;
	case 'o':
	case 'O':
		switch (raw[1]) {
		case 'n': // on
		case 'N':
			return true;
		}
		break;
	}
	return false;
}


double ghLib::INI::File::getDouble(const std::string& key) const {
	auto raw = getString(key);
	char* end;
	// TODO: deal with locale stuff - should this use a locale-independent format?
	errno = 0;
	auto value = strtod(raw.c_str(), &end);
	if (errno == 0 && *end == '\0') {
		return value;
	}
	// TODO: throw exception?
	return 0.0;
}


float ghLib::INI::File::getFloat(const std::string& key) const {
	return (float)getDouble(key);
}


int32_t ghLib::INI::File::getInt32(const std::string& key) const {
	auto raw = getString(key);
	char* end;
	errno = 0;
	auto value = strtol(raw.c_str(), &end, 0);
	if (errno == 0 && *end == '\0' && value <= INT32_MAX && value >= INT32_MIN) {
		return (int32_t)value;
	}
	warn(("bad int32 '" + raw + "'").c_str());
	// TODO: exception?
	return 0;
}


uint32_t ghLib::INI::File::getUInt32(const std::string& key) const {
	auto raw = getString(key);
	char* end;
	errno = 0;
	auto value = strtoul(raw.c_str(), &end, 0);
	if (errno == 0 && *end == '\0' && value <= UINT32_MAX) {
		return (uint32_t)value;
	}
	warn(("bad uint32 '" + raw + "'").c_str());
	// TODO: exception?
	return 0;
}


void ghLib::INI::File::setBool(const std::string& key, bool value) {
	setString(key, value ? "true" : "false");
}


void ghLib::INI::File::setDouble(const std::string& key, double value) {
	char raw[30];
	snprintf(raw, sizeof(raw), "%.17g", value);
	setString(key, raw);
}


void ghLib::INI::File::setFloat(const std::string& key, float value) {
	char raw[20];
	snprintf(raw, sizeof(raw), "%.9g", value);
	setString(key, raw);
}


void ghLib::INI::File::setInt32(const std::string& key, int32_t value) {
	char raw[20];
	snprintf(raw, sizeof(raw), "%" PRId32, value);
	setString(key, raw);
}


void ghLib::INI::File::initBool(const std::string& key, bool defaultValue) {
	if (has(key)) {
		update(key);
	} else {
		setBool(key, defaultValue);
	}
}


void ghLib::INI::File::initString(const std::string& key, const std::string& defaultValue) {
	if (has(key)) {
		update(key);
	} else {
		setString(key, defaultValue);
	}
}


void ghLib::INI::File::initDouble(const std::string& key, double defaultValue) {
	if (has(key)) {
		update(key);
	} else {
		setDouble(key, defaultValue);
	}
}


void ghLib::INI::File::initFloat(const std::string& key, float defaultValue) {
	if (has(key)) {
		update(key);
	} else {
		setFloat(key, defaultValue);
	}
}


void ghLib::INI::File::initInt32(const std::string& key, int32_t defaultValue) {
	if (has(key)) {
		update(key);
	} else {
		setInt32(key, defaultValue);
	}
}


std::string ghLib::INI::File::getString(const std::string& key, const std::string& defaultValue) const {
	return has(key) ? getString(key) : defaultValue;
}


bool ghLib::INI::File::getBool(const std::string& key, bool defaultValue) const {
	return has(key) ? getBool(key) : defaultValue;
}


double ghLib::INI::File::getDouble(const std::string& key, double defaultValue) const {
	return has(key) ? getDouble(key) : defaultValue;
}


float ghLib::INI::File::getFloat(const std::string& key, float defaultValue) const {
	return has(key) ? getFloat(key) : defaultValue;
}


int32_t ghLib::INI::File::getInt32(const std::string& key, int32_t defaultValue) const {
	return has(key) ? getInt32(key) : defaultValue;
}

uint32_t ghLib::INI::File::getUInt32(const std::string& key, uint32_t defaultValue) const {
	return has(key) ? getUInt32(key) : defaultValue;
}