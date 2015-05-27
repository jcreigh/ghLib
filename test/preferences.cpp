/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Preferences.h"
#include "gtest/gtest.h"
#include <chrono>
#include <thread>

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
	ASSERT_EQ(pref->GetInt("Moo", 5), 5);
	pref->PutInt("Moo", 10);
	ASSERT_EQ(pref->GetInt("Moo"), 10);
}


