/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include <iostream>
#include "ghLib/Logger.h"

namespace ghLib {

std::unordered_map<std::string, Logger*> Logger::loggers;
std::chrono::steady_clock::time_point Logger::startTime = std::chrono::steady_clock::now();
std::string Logger::levelNames[7] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "DISABLED"};
std::map<std::string, Logger::Level> Logger::levelMap = {
	{"TRACE", Logger::Level::TRACE},
	{"DEBUG", Logger::Level::DEBUG},
	{"INFO", Logger::Level::INFO},
	{"WARN", Logger::Level::WARN},
	{"ERROR", Logger::Level::ERROR},
	{"FATAL", Logger::Level::FATAL},
	{"DISABLED", Logger::Level::DISABLED}
};

std::string RotateFile(std::string path, int n = 0) {
	std::string newPath = path + (n > 0 ? ghLib::Format(".%d", n) : "");
	if (ghLib::Util::FS::exists(newPath)) {
		ghLib::Util::FS::rename(path, RotateFile(path, n + 1));
	}
	return newPath;
}

Logger::Entry::Entry(Logger::Level level, std::string text, Logger* baseLogger, ghLib::Clock::duration time)
: level(level), text(text), time(time), baseLogger(baseLogger) {}

Logger::Entry::Entry(Logger::Level level, std::string text, Logger* baseLogger)
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

Logger* Logger::Entry::GetLogger() {
	return baseLogger;
}

std::string Logger::Entry::GetOutput() {
	std::string outMsg = "[" + Logger::levelNames[level] + "] " + (baseLogger->GetName().size() ? ("[" + baseLogger->GetName() + "] ") : "") + text + "\n";
	if (baseLogger->IsShowingTimestamps()) {
		outMsg = ghLib::Format("[%.3f] ", (double)(std::chrono::duration_cast<std::chrono::milliseconds>(GetTime().time_since_epoch()).count()) / 1000) + outMsg;
	}
	return outMsg;
}

std::ostream& operator<<(std::ostream& os, Logger::Entry& entry) {
	os << entry.GetOutput();
	return os;
}

std::ostream* Logger::CreateFileLogger(std::string path) {
	std::string parentPath = ghLib::Util::FS::parent_path(path);
	if (parentPath != "" && !ghLib::Util::FS::is_directory(parentPath)) {
		if (!ghLib::Util::FS::create_directories(parentPath)) {
			// Failed to create the log directory, so just return a stringstream to eat everything.
			// There's probably something better which will actually eat it
			// Maybe output something to stderr?
			return new std::stringstream();
		}
	}
	return new std::ofstream(RotateFile(path));
}

Logger* Logger::GetLogger(std::string name /*= ""*/) {
	if (loggers.count(name) == 0) {
		auto p = name.rfind(".");
		std::string parent = (p != std::string::npos) ? name.substr(0, p) : "";
		Logger* parentLog = (name != "") ? GetLogger(parent) : nullptr;
		loggers[name] = new Logger(name, parentLog);
	}
	return loggers[name];
}

Logger::Logger(std::string name, Logger* parent /*= nullptr*/) : name(name), parent(parent), verbosity(Level::TRACE), timestamps(true) {
	auto pref = ghLib::Preferences::GetInstance();
	std::string key = "logger.log" + (name.size() > 0 ? ("." + name) : "");
	if (pref->ContainsKey(key)) {
		auto level = ghLib::Logger::levelMap.find(pref->GetString(key));
		if (level != ghLib::Logger::levelMap.end()) {
			verbosity = level->second;
			return;
		}
	}
	if (pref->ContainsKey("logger.default")) {
		auto level = ghLib::Logger::levelMap.find(pref->GetString("logger.default"));
		if (level != ghLib::Logger::levelMap.end()) {
			verbosity = level->second;
		}
	}
}

void Logger::Log(Level level, std::string msg, Logger* baseLogger /*= nullptr*/) {
	if (baseLogger == nullptr) {
		baseLogger = this;
	}

	auto entry = Entry(level, msg, baseLogger); // Always store the entry, despite log level
	entries.push_back(entry);

	if (level < verbosity || level == Level::DISABLED) {
		return;
	}

	for (auto& out : outputs) {
		(*out) << entry << std::flush;
	}

	if (parent) {
		parent->Log(level, msg, baseLogger);
	}
}

std::vector<Logger::Entry> Logger::GetAllEntries() {
	return entries;
}

std::vector<Logger::Entry> Logger::GetEntries(int start /* = 0 */, int count /* = -1 */, Logger::Level verbosity_ /* = Level::DISABLED */) {
	if (count < 0) { // If count < 0, then we want to show the entries up to the end
		count = (int)entries.size();
	}
	if (start < 0) { // If start < 0, we want to start counting back from the most recent entry
		start = (int)entries.size() - start;
	}
	if (verbosity_ == Level::DISABLED) { // If verbosity_ isn't specified, use the one set in the instance
		verbosity_ = verbosity;
	}

	std::vector<Logger::Entry> out;
	for (int i = 0; ((start + i) < (int)entries.size()) && (i < count); i++) {
		auto entry = entries[start + i];
		auto level = entry.GetLevel();
		if (level >= verbosity_ && level != Level::DISABLED) {
			out.push_back(entry);
		}
	}
	return out;
}

size_t Logger::Count() {
	return entries.size();
}

void Logger::DumpEntries(std::ostream& os, int start /* = 0 */, int count /* = -1 */, Logger::Level verbosity_ /* = Level::DISABLED */) {
	for (auto &e : GetEntries(start, count, verbosity_)) {
		os << e;
	}
}

std::ostream& operator<<(std::ostream& os, Logger& logger) {
	logger.DumpEntries(os);
	return os;
}

Logger* Logger::GetSubLogger(std::string subName) {
	return GetLogger((name.size() > 0 ? (name + ".") : "") + subName);
}

void Logger::ShowTimestamps(bool newValue) {
	timestamps = newValue;
}

bool Logger::IsShowingTimestamps() {
	return timestamps;
}

std::string Logger::GetName() {
	return name;
}

Logger* Logger::GetParent() {
	return parent;
}

Logger::Level Logger::GetVerbosity() {
	return verbosity;
}

void Logger::SetVerbosity(Logger::Level newLevel) {
	verbosity = newLevel;
}

void Logger::AddOutputStream(std::ostream* out) {
	outputs.push_back(out);
}

void Logger::AddOutputStream(std::ostream& out) {
	AddOutputStream(&out);
}

void Logger::DelOutputStream(std::ostream* out) {
	outputs.erase(std::remove(outputs.begin(), outputs.end(), out), outputs.end());
}

void Logger::DelOutputStream(std::ostream& out) {
	DelOutputStream(&out);
}

void Logger::Trace(std::string msg) {
	Log(Level::TRACE, msg);
}

void Logger::Debug(std::string msg) {
	Log(Level::DEBUG, msg);
}

void Logger::Info(std::string msg) {
	Log(Level::INFO, msg);
}

void Logger::Warn(std::string msg) {
	Log(Level::WARN, msg);
}

void Logger::Error(std::string msg) {
	Log(Level::ERROR, msg);
}

void Logger::Fatal(std::string msg) {
	Log(Level::FATAL, msg);
}

}
