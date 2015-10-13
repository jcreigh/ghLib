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
std::vector<Logger::Entry> Logger::entries;
std::vector<Logger::View*> Logger::views;

std::string RotateFile(std::string path, int n = 0) {
	std::string newPath = path + (n > 0 ? ghLib::Format(".%d", n) : "");
	if (ghLib::Util::FS::exists(newPath)) {
		ghLib::Util::FS::rename(path, RotateFile(path, n + 1));
	}
	return newPath;
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

void Logger::Reset() {
	entries.clear();
}

Logger::Logger(std::string name, Logger* parent /*= nullptr*/) : name(name), parent(parent), verbosity(Logger::Level::INHERIT) {
	auto pref = ghLib::Preferences::GetInstance();
	std::string key = "logger.log" + (name.size() > 0 ? ("." + name) : "");
	if (pref->ContainsKey(key)) {
		auto level = levelMap.find(pref->GetString(key));
		if (level != levelMap.end()) {
			verbosity = level->second;
			printf("Initializing logger \"%s\" at %s (from pref)\n", name.c_str(), levelNames[verbosity].c_str());
			return;
		}
	}
	if (pref->ContainsKey("logger.default")) {
		auto level = levelMap.find(pref->GetString("logger.default"));
		if (level != levelMap.end()) {
			verbosity = level->second;
		}
	}
	printf("Initializing logger \"%s\" at %s\n", name.c_str(), levelNames[verbosity].c_str());
}

void Logger::Log(Logger::Level level, std::string msg, Logger* baseLogger /*= nullptr*/) {
	loggingMutex.lock();

	if (baseLogger == nullptr) {
		baseLogger = this;
	}

	auto verbosity_ = GetVerbosity();

	if (level < verbosity_ || verbosity_ == Level::DISABLED) {
		loggingMutex.unlock();
		return;
	}

	auto entry = Entry(level, msg, baseLogger);
	entries.push_back(entry);
	int newestID = (int)entries.size() - 1;
	loggingMutex.unlock();

	for (auto& view : views) {
		view->Notify(newestID);
	}

}

Logger* Logger::GetSubLogger(std::string subName) {
	return GetLogger((name.size() > 0 ? (name + ".") : "") + subName);
}

std::string Logger::GetName() {
	return name;
}

Logger* Logger::GetParent() {
	return parent;
}

Logger::Level Logger::GetVerbosity() {
	if (verbosity == Logger::Level::INHERIT) {
		if (parent != nullptr) {
			return parent->GetVerbosity();
		} else { // We're the root logger, if we don't have anything set, default to TRACE
			return Level::TRACE;
		}
	}
	return verbosity;
}

void Logger::SetVerbosity(Logger::Level newLevel) {
	verbosity = newLevel;
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
