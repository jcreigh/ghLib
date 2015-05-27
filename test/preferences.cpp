/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Preferences.h"
#include "gtest/gtest.h"
#include <chrono>
#include <thread>

TEST(Preferences, IsKeyAcceptable) {
	ASSERT_TRUE(ghLib::Preferences::IsKeyAcceptable("foo.bar"));
	ASSERT_TRUE(ghLib::Preferences::IsKeyAcceptable("foobar"));
	ASSERT_TRUE(ghLib::Preferences::IsKeyAcceptable("foo/bar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo[bar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo]bar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo=bar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo bar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo\rbar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo\nbar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foo\tbar"));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable("foobar "));
	ASSERT_FALSE(ghLib::Preferences::IsKeyAcceptable(" foobar"));

}

TEST(Preferences, ContainsKey_No) {
	auto pref = ghLib::Preferences::GetInstance();
	ASSERT_FALSE(pref->ContainsKey("test.ContainsKey"));
}

TEST(Preferences, ContainsKey_Yes) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutBoolean("test.containsKey", true);
	ASSERT_TRUE(pref->ContainsKey("test.containsKey"));
}


TEST(Preferences, Int) {
	auto pref = ghLib::Preferences::GetInstance();
	//ASSERT_EQ(pref->GetInt("test.Moo", 5), 5);
	pref->PutInt("test.Moo", 10);
	ASSERT_EQ(pref->GetInt("test.Moo"), 10);
}


