#include <stdio.h>
#include <string>
#include "networktables/NetworkTable.h"
#include "networktables2/server/NetworkTableServer.h"

#include <gtest/gtest.h>

NetworkTableServer* server;
ITable* testTable;

int main(int argc, char **argv) {
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

	printf("Initializing tests\n");
	::testing::InitGoogleTest(&argc, argv);
	printf("Running tests\n");
	auto res = RUN_ALL_TESTS();

	sleep(1);
	printf("Shutting down\n");

	//testTable->RemoveTableListener(listener);
	NetworkTable::Shutdown();
	//delete listener;

	return res;
}
