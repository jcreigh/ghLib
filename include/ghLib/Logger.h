/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_LOGGER_H_
#define GHLIB_LOGGER_H_

#include <map>
#include <vector>
#include <ostream>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <algorithm>
#include <unordered_map>
#include <sys/stat.h>
#include "ghLib/Util.h"
#include "ghLib/Preferences.h"

#ifndef GHLIB_LOGGER_BASEPATH
#define GHLIB_LOGGER_BASEPATH ""
#endif

namespace ghLib {

class Logger {
	public:
		enum Level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, DISABLED, INHERIT };
		static std::string levelNames[8];
		static std::map<std::string, Level> levelMap;

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
				ghLib::Logger* GetLogger();
				std::string GetOutput(bool showTimestamp = true);
				friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
		};

		class View {
			private:
				std::vector<int> entryIDs;
				std::vector<std::ostream*> outputs;
				std::map<std::string, Level> levels;
				Level defaultVerbosity;
				bool showTimestamps;
				void Rescan();
			public:
				View(Level verbosity = Level::WARN, bool showTimestamps = true) /*: defaultVerbosity(verbosity), showTimestamps(showTimestamps)*/;
				~View();
				std::vector<Entry> GetEntries(int start = 0, int count = -1);
				void DumpEntries(std::ostream* out, int start = 0, int count = -1);
				void DumpEntries(std::ostream& out, int start = 0, int count = -1);
				void DumpEntriesToAll(int start = 0, int count = -1);
				void Notify(int newID, bool silent = false);
				void ShowTimestamps(bool newVal);
				void SetVerbosity(std::string loggerName, Level newLevel);
				Level GetVerbosity(std::string loggerName);
				Level GetVerbosity(Logger* logger);
				void AddOutputStream(std::ostream* out);
				void AddOutputStream(std::ostream& out);
				void DelOutputStream(std::ostream* out);
				void DelOutputStream(std::ostream& out);
		};

		static std::vector<Entry> entries;
		static std::vector<View*> views;

	private:
		static std::unordered_map<std::string, Logger*> loggers;
		static std::chrono::steady_clock::time_point startTime;
		Logger* parent;
		std::string name;
		std::vector<std::ostream*> outputs;
		Level verbosity;
		Logger(std::string name, Logger* parent = nullptr) /*: name(name), parent(parent), verbosity(Level::INHERIT), timestamps(true)*/;
		std::mutex loggingMutex;

	public:
		static std::ostream* CreateFileLogger(std::string path);
		static Logger* GetLogger(std::string name = "");
		static void Reset();

		void Log(Level level, std::string msg, Logger* baseLogger = nullptr);
		Logger* GetSubLogger(std::string subName);
		std::string GetName();
		Logger* GetParent();
		Level GetVerbosity();
		void SetVerbosity(Level newLevel);
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
