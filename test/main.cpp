/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string>
#include "ntcore.h"
#include "tables/ITableListener.h"
#include "networktables/NetworkTable.h"

#include <gtest/gtest.h>
#include "ghLib/Logger.h"

int main(int argc, char **argv) {
  nt::StartServer("persistent.ini", "", 10000);
  std::this_thread::sleep_for(std::chrono::seconds(1));

	auto log = ghLib::Logger::getLogger("Debug");
	auto view = new ghLib::Logger::View(ghLib::Logger::Level::DISABLED);
	view->addOutputStream(std::cout);
	view->setVerbosity("Debug", ghLib::Logger::TRACE);

	log->info("Initializing tests");
	::testing::InitGoogleTest(&argc, argv);

	log->info("Running tests");
	auto res = RUN_ALL_TESTS();

	NetworkTable::Shutdown();

	return res;
}
