/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Logger.h"

namespace ghLib {

Logger::Entry::Entry(Level level, std::string text, ghLib::Logger* baseLogger, ghLib::Clock::duration time)
: level(level), text(text), time(time), baseLogger(baseLogger) {}

Logger::Entry::Entry(Level level, std::string text, ghLib::Logger* baseLogger)
	: level(level), text(text), baseLogger(baseLogger) {
	time = std::chrono::duration_cast<ghLib::Clock::duration>(std::chrono::steady_clock::now() - ghLib::Clock::started);
}

Logger::Level Logger::Entry::GetLevel() {
	return level;
}

std::string Logger::Entry::GetText() {
	return text;
}

ghLib::Clock::time_point Logger::Entry::GetTime() {
	return ghLib::Clock::time_point(std::chrono::duration_cast<ghLib::Clock::duration>(ghLib::Clock::epoch.time_since_epoch())) + time;
}

ghLib::Logger* Logger::Entry::GetLogger() {
	return baseLogger;
}

std::string Logger::Entry::GetOutput(bool showTimestamp /* = true */) {
	std::string outMsg = "[" + levelNames[level] + "] " + ((baseLogger && baseLogger->GetName().size()) ? ("[" + baseLogger->GetName() + "] ") : "") + text + "\n";
	if (showTimestamp) {
		outMsg = ghLib::Format("[%.3f] ", (double)(std::chrono::duration_cast<std::chrono::milliseconds>(GetTime().time_since_epoch()).count()) / 1000) + outMsg;
	}
	return outMsg;
}

std::ostream& operator<<(std::ostream& os, Logger::Entry& entry) {
	os << entry.GetOutput();
	return os;
}

}
