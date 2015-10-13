/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Logger.h"

namespace ghLib {

Logger::View::View(Logger::Level verbosity /* = Logger::Level::WARN */, bool showTimestamps /* = true */) : defaultVerbosity(verbosity), showTimestamps(showTimestamps){
	Rescan();
	ghLib::Logger::views.push_back(this);
}

Logger::View::~View() {
	ghLib::Logger::views.erase(std::remove(ghLib::Logger::views.begin(), ghLib::Logger::views.end(), this), ghLib::Logger::views.end());
}

void Logger::View::DumpEntries(std::ostream* out, int start /* = 0 */, int count /* = -1 */) {
	auto entries = GetEntries(start, count);
	for (auto& entry : entries) {
		(*out) << entry.GetOutput(showTimestamps);
	}
}

void Logger::View::DumpEntries(std::ostream& out, int start /* = 0 */, int count /* = -1 */) {
	DumpEntries(&out, start, count);
}

void Logger::View::DumpEntriesToAll(int start /* = 0 */, int count /* = -1 */) {
	for (auto& out : outputs) {
		DumpEntries(out, start, count);
	}
}

std::vector<Logger::Entry> Logger::View::GetEntries(int start /* = 0 */, int count /* = -1 */) {
	if (count < 0) { // If count < 0, then we want to show the entries up to the end
		count = (int)entryIDs.size();
	}
	if (start < 0) { // If start < 0, we want to start counting back from the most recent entry
		start = (int)entryIDs.size() - start;
	}

	std::vector<Logger::Entry> out;
	for (int i = 0; ((start + i) < (int)entryIDs.size()) && (i < count); i++) {
		out.push_back(ghLib::Logger::entries[entryIDs[start + i]]);
	}
	return out;
}

void Logger::View::Notify(int newID, bool silent /* = false */) {
	if (newID < 0 || newID >= (int)ghLib::Logger::entries.size()) {
		return;
	}
	auto entry = ghLib::Logger::entries[newID];
	auto loggerVerbosity = GetVerbosity(entry.GetLogger());
	if (entry.GetLevel() >= loggerVerbosity && loggerVerbosity != ghLib::Logger::Level::DISABLED) {
		entryIDs.push_back(newID);
		if (!silent) {
			for (auto& out : outputs) {
				(*out) << entry.GetOutput(showTimestamps);
			}
		}
	}
}

void Logger::View::Rescan() {
	entryIDs.clear();
	for (int i = 0; i < (int)ghLib::Logger::entries.size(); i++) {
		Notify(i, true);
	}
}

void Logger::View::ShowTimestamps(bool newVal) {
	showTimestamps = newVal;
}

void Logger::View::SetVerbosity(std::string loggerName, Logger::Level newLevel) {
	levels[loggerName] = newLevel;
	Rescan();
}

Logger::Level Logger::View::GetVerbosity(ghLib::Logger* logger) {
	return GetVerbosity(logger->GetName());
}

Logger::Level Logger::View::GetVerbosity(std::string loggerName) {
	if (levels.count(loggerName) == 0) {
		return defaultVerbosity;
	}
	return levels[loggerName];
}

void Logger::View::AddOutputStream(std::ostream* out) {
	outputs.push_back(out);
}

void Logger::View::AddOutputStream(std::ostream& out) {
	AddOutputStream(&out);
}

void Logger::View::DelOutputStream(std::ostream* out) {
	outputs.erase(std::remove(outputs.begin(), outputs.end(), out), outputs.end());
}

void Logger::View::DelOutputStream(std::ostream& out) {
	DelOutputStream(&out);
}

}
