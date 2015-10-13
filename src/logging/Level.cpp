/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Logger.h"

namespace ghLib {

std::string Logger::levelNames[8] = {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "DISABLED", "INHERIT"};
std::map<std::string, Logger::Level> Logger::levelMap = {
	{"TRACE", Level::TRACE},
	{"DEBUG", Level::DEBUG},
	{"INFO", Level::INFO},
	{"WARN", Level::WARN},
	{"ERROR", Level::ERROR},
	{"FATAL", Level::FATAL},
	{"DISABLED", Level::DISABLED},
	{"INHERIT", Level::INHERIT}
};

}
