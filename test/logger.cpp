/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Logger.h"
#include "gtest/gtest.h"

TEST(Logger, CreateParents) {
	auto loggerA = ghLib::Logger::GetLogger("foo.bar.baz");
	auto loggerB = ghLib::Logger::GetLogger("foo.bar");
	ASSERT_TRUE(loggerA->GetParent() == loggerB);
	loggerA = ghLib::Logger::GetLogger("foo");
	ASSERT_TRUE(loggerA == loggerB->GetParent());
	loggerB = ghLib::Logger::GetLogger();
	ASSERT_TRUE(loggerA->GetParent() == loggerB);
}

TEST(Logger, GetParent) {
	auto logger = ghLib::Logger::GetLogger("foo.bar.baz");
	ASSERT_EQ(logger->GetName(), "foo.bar.baz");
	ASSERT_EQ(logger->GetParent()->GetName(), "foo.bar");
	ASSERT_EQ(logger->GetParent()->GetParent()->GetName(), "foo");
	ASSERT_EQ(logger->GetParent()->GetParent()->GetParent()->GetName(), "");
}

TEST(Logger, Output) {
	auto globalLogger = ghLib::Logger::GetLogger();
	auto fooLogger = ghLib::Logger::GetLogger("foo");
	auto globalView = new ghLib::Logger::View(ghLib::Logger::Level::TRACE, false);
	auto fooView = new ghLib::Logger::View(ghLib::Logger::Level::TRACE, false);
	fooView->SetVerbosity("", ghLib::Logger::Level::DISABLED);
	std::stringstream globalStream;
	std::stringstream fooStream;
	globalView->AddOutputStream(globalStream);
	fooLogger->Debug("Hello, World");
	ASSERT_EQ("[DEBUG] [foo] Hello, World\n", globalStream.str());
	fooLogger->SetVerbosity(ghLib::Logger::Level::WARN);
	fooLogger->Info("Nothing");
	ASSERT_EQ("[DEBUG] [foo] Hello, World\n", globalStream.str());
	fooView->AddOutputStream(fooStream);
	fooLogger->SetVerbosity(ghLib::Logger::Level::INFO);
	fooLogger->Error("Quack");
	ASSERT_EQ("[DEBUG] [foo] Hello, World\n[ERROR] [foo] Quack\n", globalStream.str());
	ASSERT_EQ("[ERROR] [foo] Quack\n", fooStream.str());
	globalLogger->Fatal("Meow");
	ASSERT_EQ("[DEBUG] [foo] Hello, World\n[ERROR] [foo] Quack\n[FATAL] Meow\n", globalStream.str());
	ASSERT_EQ("[ERROR] [foo] Quack\n", fooStream.str());
	delete globalView;
	delete fooView;
}

TEST(Logger, StoredEntries) {
	auto logger = ghLib::Logger::GetLogger("bar");
	auto view = new ghLib::Logger::View(ghLib::Logger::Level::DISABLED, false);
	view->SetVerbosity("bar", ghLib::Logger::Level::TRACE);
	logger->Fatal("Fatal entry");
	logger->Debug("Debug entry");
	logger->Trace("Trace entry");
	std::stringstream os;
	view->DumpEntries(os);
	ASSERT_EQ("[FATAL] [bar] Fatal entry\n[DEBUG] [bar] Debug entry\n[TRACE] [bar] Trace entry\n", os.str());
	os.str("");
	view->SetVerbosity("bar", ghLib::Logger::Level::WARN);
	view->DumpEntries(os);
	//printf("|%s|\n", os.str().c_str());
	ASSERT_EQ("[FATAL] [bar] Fatal entry\n", os.str());
	delete view;
}
