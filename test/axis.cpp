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

TEST(Axis, StructConstructor) {
	ghLib::Axis::Config c;
	c.config = "Axis/StructConstructor"; c.channel = 5; c.js = 2; c.deadband = 0.05f; c.invert = true;
	auto axis = ghLib::Axis(c);
	auto stick = ghLib::Joystick::GetStickForPort(2);
	stick->SetRawAxis(5, 0.5f);
	ASSERT_NEAR(-0.47368419f, axis.Get(), 0.0001f);
	stick->SetRawAxis(5, 0.01f);
	ASSERT_NEAR(axis.Get(), 0.0f, 0.0001f);
	ASSERT_NEAR(axis.GetDeadband(), 0.05f, 0.0001f);
}

TEST(Axis, StructLambdaConstructor) {
	auto axis = ghLib::Axis([](auto& c) { c.config = "Axis/StructLambdaConstructor"; c.channel = 5; c.js = 2; c.deadband = 0.05f; c.invert = true;});
	auto stick = ghLib::Joystick::GetStickForPort(2);
	stick->SetRawAxis(5, 0.5f);
	ASSERT_NEAR(-0.47368419f, axis.Get(), 0.0001f);
	stick->SetRawAxis(5, 0.01f);
	ASSERT_NEAR(axis.Get(), 0.0f, 0.0001f);
	ASSERT_NEAR(axis.GetDeadband(), 0.05f, 0.0001f);
}

TEST(Axis, PrefConstructor) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Axis/type", "axis");
	pref->PutNumber("test/Axis/channel", 5);
	pref->PutNumber("test/Axis/js", 2);
	pref->PutNumber("test/Axis/deadband", 0.05f);
	pref->PutBoolean("test/Axis/invert", true);
	auto axis = ghLib::Axis("test/Axis");
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

TEST(Axis, Virtual) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Virtual/A/type", "axis");
	pref->PutNumber("test/Virtual/A/channel", 1);
	pref->PutString("test/Virtual/B/type", "axis");
	pref->PutString("test/Virtual/B/src", "virtual");
	pref->PutString("test/Virtual/B/virtual", "test/Virtual/A");
	pref->PutBoolean("test/Virtual/B/invert", true);
	auto stick = ghLib::Joystick::GetStickForPort(0);
	stick->SetRawAxis(1, 0.75f);
	auto axis = ghLib::Axis("test/Virtual/A");
	auto axis2 = ghLib::Axis("test/Virtual/B");
	ASSERT_NEAR(0.75f, axis.Get(), 0.0001f);
	ASSERT_NEAR(-0.75f, axis2.Get(), 0.0001f);

}

TEST(Axis, Coerce) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Coerce/type", "axis");
	pref->PutNumber("test/Coerce/channel", 1);
	pref->PutNumber("test/Coerce/input/min", 0.1f);
	pref->PutNumber("test/Coerce/input/max", 0.6f);
	auto axis = ghLib::Axis("test/Coerce");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	stick->SetRawAxis(1, -0.1f);
	ASSERT_NEAR(0.1f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.5f);
	ASSERT_NEAR(0.5f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.8f);
	ASSERT_NEAR(0.6f, axis.Get(), 0.0001f);
}

TEST(Axis, Interpolate) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Interpolate/type", "axis");
	pref->PutNumber("test/Interpolate/channel", 1);
	pref->PutNumber("test/Interpolate/input/min", 0.0f);
	pref->PutNumber("test/Interpolate/input/max", 0.5f);
	pref->PutNumber("test/Interpolate/output/min", 0.0f);
	pref->PutNumber("test/Interpolate/output/max", 4095.0f);
	pref->PutBoolean("test/Interpolate/scale", true);
	auto axis = ghLib::Axis("test/Interpolate");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	stick->SetRawAxis(1, -0.1f);
	ASSERT_NEAR(0.0f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.5f);
	ASSERT_NEAR(4095.0f, axis.Get(), 0.0001f);
	stick->SetRawAxis(1, 0.4f);
	ASSERT_NEAR(3276.0f, axis.Get(), 0.0001f);
}

TEST(Axis, AnalogInput) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Analog/type", "axis");
	pref->PutNumber("test/Analog/channel", 1);
	pref->PutString("test/Analog/src", "analog");
	auto axis = ghLib::Axis("test/Analog");
	auto analog = new ghLib::AnalogInput(1);
	analog->SetValue(4095);
	ASSERT_NEAR(1.0f, axis.Get(), 0.0001f);
	analog->SetValue(0);
	ASSERT_NEAR(-1.0f, axis.Get(), 0.0001f);
	analog->SetValue(1000);
	ASSERT_NEAR(-0.51159f, axis.Get(), 0.0001f);
}
