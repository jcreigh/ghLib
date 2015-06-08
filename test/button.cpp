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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonRawTest", 5);
	pref->PutInt("test.buttonRawTest.js", 2);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonToggleTest", 6);
	pref->PutInt("test.buttonToggleTest.js", 3);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonPressTest", 1);
	pref->PutInt("test.buttonPressTest.js", 0);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonReleaseTest", 1);
	pref->PutInt("test.buttonReleaseTest.js", 0);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonRawPovTest", 3);
	pref->PutInt("test.buttonRawPovTest.js", 0);
	pref->PutString("test.buttonRawPovTest.mode", "raw");
	pref->PutString("test.buttonRawPovTest.type", "pov");
	pref->PutInt("test.buttonRawPovTest.pov", 2);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonRawAxisPosTest", 3);
	pref->PutInt("test.buttonRawAxisPosTest.js", 0);
	pref->PutString("test.buttonRawAxisPosTest.mode", "raw");
	pref->PutString("test.buttonRawAxisPosTest.type", "axis");
	pref->PutFloat("test.buttonRawAxisPosTest.threshold", 0.6f);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonRawAxisNegTest", 3);
	pref->PutInt("test.buttonRawAxisNegTest.js", 0);
	pref->PutString("test.buttonRawAxisNegTest.mode", "raw");
	pref->PutString("test.buttonRawAxisNegTest.type", "axis");
	pref->PutFloat("test.buttonRawAxisNegTest.threshold", -0.6f);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonVirtualTest", 3);
	pref->PutInt("test.buttonVirtualTest.js", 0);
	pref->PutString("test.buttonVirtualTest.mode", "press");
	pref->PutString("test.buttonVirtualTest.type", "virtual");
	pref->PutString("test.buttonVirtualTest.virtual", "test.otherButton");
	pref->PutInt("test.otherButton", 4);
	pref->PutInt("test.otherButton.js", 0);
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
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("test.buttonInvertTest", 6);
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

