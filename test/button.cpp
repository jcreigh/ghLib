/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
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


TEST(Button, RawButton) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutNumber("test.buttonRawTest", 5);
	pref->PutNumber("test.buttonRawTest.js", 2);
	pref->PutString("test.buttonRawTest.mode", "raw");
	pref->PutString("test.buttonRawTest.type", "button");
	auto button = ghLib::Button("test.buttonRawTest");
	auto stick = ghLib::Joystick::GetStickForPort(2);
	ghLib::ButtonRunner::SetEnabled(false);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true); button.Update();
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}

TEST(Button, ToggleButton) {
	auto pref = NetworkTable::GetTable("Preferences");
	pref->PutNumber("test.buttonToggleTest", 6);
	pref->PutNumber("test.buttonToggleTest.js", 3);
	pref->PutString("test.buttonToggleTest.mode", "toggle");
	pref->PutString("test.buttonToggleTest.type", "button");
	auto button = ghLib::Button("test.buttonToggleTest");
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
	pref->PutNumber("test.buttonPressTest", 1);
	pref->PutNumber("test.buttonPressTest.js", 0);
	pref->PutString("test.buttonPressTest.mode", "press");
	pref->PutString("test.buttonPressTest.type", "button");
	auto button = ghLib::Button("test.buttonPressTest");
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
	pref->PutNumber("test.buttonReleaseTest", 1);
	pref->PutNumber("test.buttonReleaseTest.js", 0);
	pref->PutString("test.buttonReleaseTest.mode", "release");
	pref->PutString("test.buttonReleaseTest.type", "button");
	auto button = ghLib::Button("test.buttonReleaseTest");
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
	pref->PutNumber("test.buttonRawPovTest", 3);
	pref->PutNumber("test.buttonRawPovTest.js", 0);
	pref->PutString("test.buttonRawPovTest.mode", "raw");
	pref->PutString("test.buttonRawPovTest.type", "pov");
	pref->PutNumber("test.buttonRawPovTest.pov", 2);
	auto button = ghLib::Button("test.buttonRawPovTest");
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
	pref->PutNumber("test.buttonRawAxisPosTest", 3);
	pref->PutNumber("test.buttonRawAxisPosTest.js", 0);
	pref->PutString("test.buttonRawAxisPosTest.mode", "raw");
	pref->PutString("test.buttonRawAxisPosTest.type", "axis");
	pref->PutNumber("test.buttonRawAxisPosTest.threshold", 0.6f);
	auto button = ghLib::Button("test.buttonRawAxisPosTest");
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
	pref->PutNumber("test.buttonRawAxisNegTest", 3);
	pref->PutNumber("test.buttonRawAxisNegTest.js", 0);
	pref->PutString("test.buttonRawAxisNegTest.mode", "raw");
	pref->PutString("test.buttonRawAxisNegTest.type", "axis");
	pref->PutNumber("test.buttonRawAxisNegTest.threshold", -0.6f);
	auto button = ghLib::Button("test.buttonRawAxisNegTest");
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
	pref->PutNumber("test.buttonVirtualTest", 3);
	pref->PutNumber("test.buttonVirtualTest.js", 0);
	pref->PutString("test.buttonVirtualTest.mode", "press");
	pref->PutString("test.buttonVirtualTest.type", "virtual");
	pref->PutString("test.buttonVirtualTest.virtual", "test.otherButton");
	pref->PutNumber("test.otherButton", 4);
	pref->PutNumber("test.otherButton.js", 0);
	pref->PutString("test.otherButton.mode", "toggle");
	pref->PutString("test.otherButton.type", "button");
	auto other = ghLib::Button("test.otherButton");
	auto button = ghLib::Button("test.buttonVirtualTest");
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
	pref->PutNumber("test.buttonInvertTest", 6);
	pref->PutBoolean("test.buttonInvertTest.invert", true);
	auto button = ghLib::Button("test.buttonInvertTest");
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

	pref->PutNumber("test.Button.VirtualAxis.axis", 0);
	auto axis = ghLib::Axis("test.Button.VirtualAxis.axis");

	pref->PutNumber("test.Button.VirtualAxis.button", 0);
	pref->PutString("test.Button.VirtualAxis.button.type", "virtual");
	pref->PutNumber("test.Button.VirtualAxis.button.threshold", 0.6f);
	pref->PutString("test.Button.VirtualAxis.button.virtual", "test.Button.VirtualAxis.axis");
	auto button = ghLib::Button("test.Button.VirtualAxis.button");

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
	pref->PutNumber("test.Button.analog", 1);
	pref->PutString("test.Button.analog.type", "analog");
	pref->PutNumber("test.Button.analog.threshold", 0.6f);
	auto button = ghLib::Button("test.Button.analog");
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
	pref->PutNumber("test.Button.digital", 1);
	pref->PutString("test.Button.digital.type", "digital");
	auto button = ghLib::Button("test.Button.digital");
	auto digital = new ghLib::DigitalInput(1);
	digital->Set(false); button.Update();
	ASSERT_FALSE(button.Get());
	digital->Set(true); button.Update();
	ASSERT_TRUE(button.Get());
	digital->Set(false); button.Update();
	ASSERT_FALSE(button.Get());
}
