/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2017. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Button.h"
#include "gtest/gtest.h"

TEST(Button, SimpleConstructor) {
	auto button = ghLib::Button(1); // Button 1 on joystick 0
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(1, false);
}

TEST(Button, ComplexConstructor) {
	auto stick = ghLib::Joystick::GetStickForPort(3);
	auto button = ghLib::Button(5, stick, ghLib::Button::kToggle);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true); button.Update(); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, true); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, false); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}

TEST(Button, StructConstructor) {
	ghLib::Button::Config c;
	c.config = "Button/StructConstructor"; c.channel = 5; c.js = 2; c.mode = "raw"; c.src = "button";
	auto button = ghLib::Button(c);
	auto stick = ghLib::Joystick::GetStickForPort(2);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}

TEST(Button, StructLambdaConstructor) {
	ghLib::Button::Config c;
	auto button = ghLib::Button([](auto& c) { c.config = "Button/StructLambdaConstructor"; c.channel = 5; c.js = 2; c.mode = "raw"; c.src = "button";});
	auto stick = ghLib::Joystick::GetStickForPort(2);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}

TEST(Button, RawButton) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/Raw/type", "button");
	pref->PutNumber("test/Button/Raw/channel", 5);
	pref->PutNumber("test/Button/Raw/js", 2);
	pref->PutString("test/Button/Raw/mode", "raw");
	pref->PutString("test/Button/Raw/src", "button");
	auto button = ghLib::Button("test/Button/Raw");
	auto stick = ghLib::Joystick::GetStickForPort(2);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}

TEST(Button, ToggleButton) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/Toggle/type", "button");
	pref->PutNumber("test/Button/Toggle/channel", 6);
	pref->PutNumber("test/Button/Toggle/js", 3);
	pref->PutString("test/Button/Toggle/mode", "toggle");
	pref->PutString("test/Button/Toggle/src", "button");
	auto button = ghLib::Button("test/Button/Toggle");
	auto stick = ghLib::Joystick::GetStickForPort(3);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(6, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(6, false); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(6, true); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(6, false); button.Update();
	ASSERT_FALSE(button.Get());
}

TEST(Button, PressButton) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/Press/type", "button");
	pref->PutNumber("test/Button/Press/channel", 1);
	pref->PutNumber("test/Button/Press/js", 0);
	pref->PutString("test/Button/Press/mode", "press");
	pref->PutString("test/Button/Press/src", "button");
	auto button = ghLib::Button("test/Button/Press");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, true); button.Update();
	ASSERT_TRUE(button.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, false); button.Update();
	ASSERT_FALSE(button.Get());
}

TEST(Button, ReleaseButton) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/Release/type", "button");
	pref->PutNumber("test/Button/Release/channel", 1);
	pref->PutNumber("test/Button/Release/js", 0);
	pref->PutString("test/Button/Release/mode", "release");
	pref->PutString("test/Button/Release/src", "button");
	auto button = ghLib::Button("test/Button/Release");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, true); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, false); button.Update();
	ASSERT_TRUE(button.Get());
	ASSERT_FALSE(button.Get());
}


TEST(Button, RawPOV) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/POV/type", "button");
	pref->PutNumber("test/Button/POV/channel", 3);
	pref->PutNumber("test/Button/POV/js", 0);
	pref->PutString("test/Button/POV/mode", "raw");
	pref->PutString("test/Button/POV/src", "pov");
	pref->PutNumber("test/Button/POV/pov", 2);
	auto button = ghLib::Button("test/Button/POV");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetPOV(2, 45); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetPOV(2, 90); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetPOV(2, 135); button.Update();
	ASSERT_TRUE(button.Get());
}

TEST(Button, RawAxisPos) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/AxisPos/type", "button");
	pref->PutNumber("test/Button/AxisPos/channel", 3);
	pref->PutNumber("test/Button/AxisPos/js", 0);
	pref->PutString("test/Button/AxisPos/mode", "raw");
	pref->PutString("test/Button/AxisPos/src", "axis");
	pref->PutNumber("test/Button/AxisPos/threshold", 0.6f);
	auto button = ghLib::Button("test/Button/AxisPos");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	stick->SetRawAxis(3, 0.0f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.25f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.5f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.75f); button.Update(); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(3, 1.0f); button.Update(); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(3, 0.59f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, -0.75f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
}

TEST(Button, RawAxisNeg) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/AxisNeg/type", "button");
	pref->PutNumber("test/Button/AxisNeg/channel", 3);
	pref->PutNumber("test/Button/AxisNeg/js", 0);
	pref->PutString("test/Button/AxisNeg/mode", "raw");
	pref->PutString("test/Button/AxisNeg/src", "axis");
	pref->PutNumber("test/Button/AxisNeg/threshold", -0.6f);
	auto button = ghLib::Button("test/Button/AxisNeg");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	stick->SetRawAxis(3, 0.0f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.25f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.5f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.75f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 1.0f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.59f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, -0.75f); button.Update(); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(3, -0.5f); button.Update(); button.Update();
	ASSERT_FALSE(button.Get());
}


TEST(Button, Virtual) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/Virtual/type", "button");
	pref->PutNumber("test/Button/Virtual/js", 0);
	pref->PutString("test/Button/Virtual/mode", "press");
	pref->PutString("test/Button/Virtual/src", "virtual");
	pref->PutString("test/Button/Virtual/virtual", "test/Button/Virtual/otherButton");
	pref->PutString("test/Button/Virtual/otherButton/type", "button");
	pref->PutNumber("test/Button/Virtual/otherButton/channel", 4);
	pref->PutNumber("test/Button/Virtual/otherButton/js", 0);
	pref->PutString("test/Button/Virtual/otherButton/mode", "toggle");
	pref->PutString("test/Button/Virtual/otherButton/src", "button");
	auto other = ghLib::Button("test/Button/Virtual/otherButton");
	auto button = ghLib::Button("test/Button/Virtual");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	stick->SetRawButton(4, false); other.Update(); button.Update();
	ASSERT_FALSE(button.Get());
	ASSERT_FALSE(other.Get());
	stick->SetRawButton(4, true); other.Update(); button.Update();
	ASSERT_TRUE(other.Get());
	ASSERT_TRUE(button.Get());
	ASSERT_TRUE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, false); other.Update(); button.Update();
	ASSERT_TRUE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, true); other.Update(); button.Update();
	ASSERT_FALSE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, false); other.Update(); button.Update();
	ASSERT_FALSE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, true); other.Update(); button.Update();
	ASSERT_TRUE(other.Get());
	ASSERT_TRUE(button.Get());
}

TEST(Button, Invert) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/Invert/type", "button");
	pref->PutNumber("test/Button/Invert/channel", 6);
	pref->PutBoolean("test/Button/Invert/invert", true);
	auto button = ghLib::Button("test/Button/Invert");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	stick->SetRawButton(6, false); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(6, true); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(6, false);
}

TEST(Button, VirtualAxis) {
	auto pref = NetworkTable::GetTable("Preferences");

	pref->PutString("test/Button/VirtualAxis/axis/type", "axis");
	pref->PutNumber("test/Button/VirtualAxis/axis/channel", 0);
	auto axis = ghLib::Axis("test/Button/VirtualAxis/axis");

	pref->PutString("test/Button/VirtualAxis/button/type", "button");
	pref->PutNumber("test/Button/VirtualAxis/button/channel", 0);
	pref->PutString("test/Button/VirtualAxis/button/src", "virtual");
	pref->PutNumber("test/Button/VirtualAxis/button/threshold", 0.6f);
	pref->PutString("test/Button/VirtualAxis/button/virtual", "test/Button/VirtualAxis/axis");
	auto button = ghLib::Button("test/Button/VirtualAxis/button");

	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(false);
	stick->SetRawAxis(0, 0.0f); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(0, 0.8f); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(0, -0.8f); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(0, 0.5f); button.Update();
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(0, 1.0f); button.Update();
	ASSERT_TRUE(button.Get());
}

TEST(Button, AnalogInput) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/analog/type", "button");
	pref->PutNumber("test/Button/analog/channel", 1);
	pref->PutString("test/Button/analog/src", "analog");
	pref->PutNumber("test/Button/analog/threshold", 0.6f);
	auto button = ghLib::Button("test/Button/analog");
	auto analog = new ghLib::AnalogInput(1);
	analog->SetValue(4095); button.Update();
	ASSERT_TRUE(button.Get());
	analog->SetValue(3000); button.Update();
	ASSERT_FALSE(button.Get());
	analog->SetValue(0); button.Update();
	ASSERT_FALSE(button.Get());
	analog->SetValue(3500); button.Update();
	ASSERT_TRUE(button.Get());
}

TEST(Button, DigitalInput) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutString("test/Button/digital/type", "button");
	pref->PutNumber("test/Button/digital/channel", 1);
	pref->PutString("test/Button/digital/src", "digital");
	auto button = ghLib::Button("test/Button/digital");
	auto digital = new ghLib::DigitalInput(1);
	digital->Set(false); button.Update();
	ASSERT_FALSE(button.Get());
	digital->Set(true); button.Update();
	ASSERT_TRUE(button.Get());
	digital->Set(false); button.Update();
	ASSERT_FALSE(button.Get());
}
