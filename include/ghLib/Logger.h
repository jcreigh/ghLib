/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_LOGGER_H_
#define GHLIB_LOGGER_H_

#include <vector>
#include <ostream>
#include <chrono>
#include <memory>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <sys/stat.h>
#include "ghLib/Util.h"

#ifndef GHLIB_LOGGER_BASEPATH
#define GHLIB_LOGGER_BASEPATH ""
#endif

namespace ghLib {

class Logger {
	public:
		enum Level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, DISABLED };
		static std::string levelNames[7];
		class Entry {
			private:
				Level level;
				std::string text;
				ghLib::Clock::duration time;
				Logger* baseLogger;
			public:
				Entry(Level level, std::string text, Logger* baseLogger, ghLib::Clock::duration time);
				Entry(Level level, std::string text, Logger* baseLogger);
				Level GetLevel();
				std::string GetText();
				ghLib::Clock::time_point GetTime();
				Logger* GetLogger();
				std::string GetOutput();
				friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
		};
	private:
		std::vector<Entry> entries;
		static std::unordered_map<std::string, Logger*> loggers;
		static std::chrono::steady_clock::time_point startTime;
		Logger* parent;
		std::string name;
		bool timestamps;
		std::vector<std::ostream*> outputs;
		Level verbosity;

		Logger(std::string name, Logger* parent = nullptr);
	public:
		static std::ostream* CreateFileLogger(std::string path);
		static Logger* GetLogger(std::string name = "");

		void Log(Level level, std::string msg, Logger* baseLogger = nullptr);
		std::vector<Entry> GetEntries();
		size_t Count();
		void DumpEntries(std::ostream& os, int start = 0, int count = -1, Level verbosity_ = Level::DISABLED);
		Logger* GetSubLogger(std::string subName);
		void ShowTimestamps(bool newValue);
		bool IsShowingTimestamps(); // TODO: Rename to something better maybe
		std::string GetName();
		Logger* GetParent();
		Level GetVerbosity();
		void SetVerbosity(Level newLevel);
		void AddOutputStream(std::ostream* out);
		void AddOutputStream(std::ostream& out);
		void DelOutputStream(std::ostream* out);
		void DelOutputStream(std::ostream& out);
		void Trace(std::string msg);
		void Debug(std::string msg);
		void Info(std::string msg);
		void Warn(std::string msg);
		void Error(std::string msg);
		void Fatal(std::string msg);

};

std::ostream& operator<<(std::ostream& os, Logger::Entry& entry);
std::ostream& operator<<(std::ostream& os, Logger& logger);

}

#endif /* GHLIB_LOGGER_H_ */
