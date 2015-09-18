/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include <stdio.h>
#include <string>
#include "networktables/NetworkTable.h"
#include "networktables2/server/NetworkTableServer.h"

#include <gtest/gtest.h>
#include "ghLib/Preferences.h"
#include "ghLib/web/Server.h"
#include "ghLib/Logger.h"

NetworkTableServer* server;
ITable* testTable;

int main(int argc, char **argv) {
	auto log = ghLib::Logger::GetLogger("Debug");
	log->SetVerbosity(ghLib::Logger::TRACE);
	NetworkTable::SetServerMode();
	NetworkTable::Initialize();

	testTable = NetworkTable::GetTable("Test");

	//class ServerListener : public ITableListener {
		//virtual void ValueChanged(ITable* source, const std::string& key, EntryValue value, bool isNew){
			//fprintf(stdout, "Got key in table: %s = %f\n", key.c_str(), value.f);
			//fflush(stdout);
		//};
	//};
	//auto listener = new ServerListener();
	//testTable->AddTableListener(listener, true);

	log->Info("Initializing tests");
	::testing::InitGoogleTest(&argc, argv);
	log->Info("Running tests");

	//ghLib::Logger::GetLogger("Button")->AddOutputStream(std::cout);
	auto res = RUN_ALL_TESTS();

	log->AddOutputStream(std::cout);

	auto ws = ghLib::web::WebServer::GetInstance();
	log->Info("Starting wobserver");
	ws->SetEnabled(true);

	sleep(60);
	log->Info("Shutting down");
	auto pref = ghLib::Preferences::GetInstance();
	pref->Save();

	//testTable->RemoveTableListener(listener);
	NetworkTable::Shutdown();
	//delete listener;

	return res;
}
