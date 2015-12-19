/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Axis.h"
#include "gtest/gtest.h"

TEST(Axis, SimpleConstructor) {
	auto axis = ghLib::Axis(1); // Axis 1 on joystick 0
	auto stick = ghLib::Joystick::GetStickForPort(0);
	stick->SetRawAxis(1, 0.0f);
	ASSERT_NEAR(0.0f, axis.Get(), 0.0001f);
}

TEST(Axis, ComplexConstructor) {
	auto stick = ghLib::Joystick::GetStickForPort(0);
	stick->SetRawAxis(1, 0.0f);
	auto axis = ghLib::Axis(1, stick); // Axis 1 on joystick 0
	ASSERT_NEAR(0.0f, axis.Get(), 0.0001f);
}


TEST(Axis, PrefConstructor) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutNumber("test.axisTest", 5);
	pref->PutNumber("test.axisTest.js", 2);
	pref->PutNumber("test.axisTest.deadband", 0.05f);
	pref->PutBoolean("test.axisTest.invert", true);
	auto axis = ghLib::Axis("test.axisTest");
	auto stick = ghLib::Joystick::GetStickForPort(2);
	stick->SetRawAxis(5, 0.5f);
	ASSERT_NEAR(-0.47368419f, axis.Get(), 0.0001f);
	stick->SetRawAxis(5, 0.01f);
	ASSERT_NEAR(axis.Get(), 0.0f, 0.0001f);
	ASSERT_NEAR(axis.GetDeadband(), 0.05f, 0.0001f);
}

TEST(Axis, Deadband) {
	auto stick = ghLib::Joystick::GetStickForPort(0);
	auto axis = ghLib::Axis(1, stick); // Axis 1 on joystick 0
	stick->SetRawAxis(1, 0.06f);
	ASSERT_NEAR(0.06f, axis.Get(), 0.0001f);
	axis.SetDeadband(0.05f);
	ASSERT_NEAR(0.010526313f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.05f);
	ASSERT_NEAR(0.0f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.01f);
	ASSERT_NEAR(0.0f, axis.Get(), 0.0001f);
}

TEST(Axis, Invert) {
	auto axis = ghLib::Axis(1); // Axis 1 on joystick 0
	auto stick = ghLib::Joystick::GetStickForPort(0);
	stick->SetRawAxis(1, 0.0f);
	ASSERT_NEAR(0.0f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.5f);
	ASSERT_NEAR(0.5f, axis.Get(), 0.0001f);
	axis.SetInvert(true);
	ASSERT_NEAR(-0.5f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.0f);
	ASSERT_NEAR(0.0f, axis.Get(), 0.001f);
}

