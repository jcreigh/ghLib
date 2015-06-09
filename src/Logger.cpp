/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Logger.h"

namespace ghLib {

std::unordered_map<std::string, Logger*> Logger::loggers;
std::chrono::steady_clock::time_point Logger::startTime = std::chrono::steady_clock::now();

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

Logger::Logger(std::string name, Logger* parent /*= nullptr*/) : name(name), parent(parent), outputLevel(Level::TRACE), timestamps(true) {}

void Logger::Log(Level level, std::string msg, std::string outName /*= ""*/) {
	if (outName.size() == 0) {
		outName = name;
	}
	if (level < outputLevel || level == Level::DISABLED) {
		return;
	}
	std::string outMsg = "[" + levelNames[level] + "] " + (outName.size() ? ("[" + outName + "] ") : "") + msg + "\n";
	if (timestamps) {
		auto ts = std::chrono::steady_clock::now() - startTime;
		outMsg = ghLib::Format("[%.3f] ", (float)(std::chrono::duration_cast<std::chrono::milliseconds>(ts).count()) / 1000) + outMsg;
	}
	for (auto out : outputs) {
		(*out) << outMsg << std::flush;
	}
	if (parent) {
		parent->Log(level, msg, outName);
	}
}

void Logger::ShowTimestamps(bool newValue) {
	timestamps = newValue;
}

std::string Logger::GetName() {
	return name;
}

Logger* Logger::GetParent() {
	return parent;
}

Logger::Level Logger::GetOutputLevel() {
	return outputLevel;
}

void Logger::SetOutputLevel(Logger::Level newLevel) {
	outputLevel = newLevel;
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
