/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include <vector>
#include <ostream>
#include <chrono>
#include <memory>
#include <string>
#include <algorithm>
#include <unordered_map>
#include "ghLib/Util.h"

namespace ghLib {

class Logger {
	public:
		enum Level { TRACE, DEBUG, INFO, WARN, ERROR, FATAL, DISABLED };
		std::string levelNames[7] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "DISABLED"};
	private:
		static std::unordered_map<std::string, Logger*> loggers;
		static std::chrono::steady_clock::time_point startTime;
		Logger* parent;
		std::string name;
		bool timestamps;
		std::vector<std::ostream*> outputs;
		Level outputLevel;

		Logger(std::string name, Logger* parent = nullptr);
	public:
		static Logger* GetLogger(std::string name = "");

		void Log(Level level, std::string msg, std::string outName = "");
		void ShowTimestamps(bool newValue);
		std::string GetName();
		Logger* GetParent();
		Level GetOutputLevel();
		void SetOutputLevel(Level newLevel);
		void AddOutputStream(std::ostream& out);
		void DelOutputStream(std::ostream& out);
		void Trace(std::string msg);
		void Debug(std::string msg);
		void Info(std::string msg);
		void Warn(std::string msg);
		void Error(std::string msg);
		void Fatal(std::string msg);

};

}
