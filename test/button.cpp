/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/Button.h"
#include "gtest/gtest.h"

void Sleep(int delay) {
	std::this_thread::sleep_for(std::chrono::milliseconds(delay));
}

TEST(Button, SimpleConstructor) {
	auto button = ghLib::Button(1); // Button 1 on joystick 0
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, true);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(1, false);
}

TEST(Button, ComplexConstructor) {
	auto stick = ghLib::Joystick::GetStickForPort(3);
	auto button = ghLib::Button(5, stick, ghLib::Button::kToggle);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, false);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}


TEST(Button, RawButton) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonRawTest", 5);
	pref->PutInt("buttonRawTest.js", 2);
	pref->PutString("buttonRawTest.mode", "raw");
	pref->PutString("buttonRawTest.type", "button");
	Sleep(50);
	auto button = ghLib::Button("buttonRawTest");
	auto stick = ghLib::Joystick::GetStickForPort(2);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(5, true);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(5, false);
}

TEST(Button, ToggleButton) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonToggleTest", 6);
	pref->PutInt("buttonToggleTest.js", 3);
	pref->PutString("buttonToggleTest.mode", "toggle");
	pref->PutString("buttonToggleTest.type", "button");
	Sleep(50);
	auto button = ghLib::Button("buttonToggleTest");
	auto stick = ghLib::Joystick::GetStickForPort(3);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(6, true);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(6, false);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(6, true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(6, false);
	Sleep(50);
	ASSERT_FALSE(button.Get());
}

TEST(Button, PressButton) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonPressTest", 1);
	pref->PutInt("buttonPressTest.js", 0);
	pref->PutString("buttonPressTest.mode", "press");
	pref->PutString("buttonPressTest.type", "button");
	Sleep(50);
	auto button = ghLib::Button("buttonPressTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, true);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, false);
	Sleep(50);
	ASSERT_FALSE(button.Get());
}

TEST(Button, ReleaseButton) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonReleaseTest", 1);
	pref->PutInt("buttonReleaseTest.js", 0);
	pref->PutString("buttonReleaseTest.mode", "release");
	pref->PutString("buttonReleaseTest.type", "button");
	Sleep(50);
	auto button = ghLib::Button("buttonReleaseTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(1, false);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	ASSERT_FALSE(button.Get());
}


TEST(Button, RawPOV) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonRawPovTest", 3);
	pref->PutInt("buttonRawPovTest.js", 0);
	pref->PutString("buttonRawPovTest.mode", "raw");
	pref->PutString("buttonRawPovTest.type", "pov");
	pref->PutInt("buttonRawPovTest.pov", 2);
	Sleep(50);
	auto button = ghLib::Button("buttonRawPovTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetPOV(2, 45);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetPOV(2, 90);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetPOV(2, 135);
	Sleep(50);
	ASSERT_TRUE(button.Get());
}

TEST(Button, RawAxisPos) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonRawAxisPosTest", 3);
	pref->PutInt("buttonRawAxisPosTest.js", 0);
	pref->PutString("buttonRawAxisPosTest.mode", "raw");
	pref->PutString("buttonRawAxisPosTest.type", "axis");
	pref->PutFloat("buttonRawAxisPosTest.threshold", 0.6f);
	Sleep(50);
	auto button = ghLib::Button("buttonRawAxisPosTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	stick->SetRawAxis(3, 0.0f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.25f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.5f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.75f);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(3, 1.0f);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(3, 0.59f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, -0.75f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
}

TEST(Button, RawAxisNeg) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonRawAxisNegTest", 3);
	pref->PutInt("buttonRawAxisNegTest.js", 0);
	pref->PutString("buttonRawAxisNegTest.mode", "raw");
	pref->PutString("buttonRawAxisNegTest.type", "axis");
	pref->PutFloat("buttonRawAxisNegTest.threshold", -0.6f);
	Sleep(50);
	auto button = ghLib::Button("buttonRawAxisNegTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	stick->SetRawAxis(3, 0.0f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.25f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.5f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.75f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 1.0f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, 0.59f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawAxis(3, -0.75f);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawAxis(3, -0.5f);
	Sleep(50);
	ASSERT_FALSE(button.Get());
}


TEST(Button, Virtual) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonVirtualTest", 3);
	pref->PutInt("buttonVirtualTest.js", 0);
	pref->PutString("buttonVirtualTest.mode", "press");
	pref->PutString("buttonVirtualTest.type", "virtual");
	pref->PutString("buttonVirtualTest.virtual", "otherButton");
	pref->PutInt("otherButton", 4);
	pref->PutInt("otherButton.js", 0);
	pref->PutString("otherButton.mode", "toggle");
	pref->PutString("otherButton.type", "button");
	Sleep(50);
	auto other = ghLib::Button("otherButton");
	auto button = ghLib::Button("buttonVirtualTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	stick->SetRawButton(4, false);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	ASSERT_FALSE(other.Get());
	stick->SetRawButton(4, true);
	Sleep(50);
	ASSERT_TRUE(other.Get());
	ASSERT_TRUE(button.Get());
	ASSERT_TRUE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, false);
	Sleep(50);
	ASSERT_TRUE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, true);
	Sleep(50);
	ASSERT_FALSE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, false);
	Sleep(50);
	ASSERT_FALSE(other.Get());
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(4, true);
	Sleep(50);
	ASSERT_TRUE(other.Get());
	ASSERT_TRUE(button.Get());
}

TEST(Button, Invert) {
	auto pref = ghLib::Preferences::GetInstance();
	pref->PutInt("buttonInvertTest", 6);
	pref->PutBoolean("buttonInvertTest.invert", true);
	Sleep(50);
	auto button = ghLib::Button("buttonInvertTest");
	auto stick = ghLib::Joystick::GetStickForPort(0);
	ghLib::ButtonRunner::SetEnabled(true);
	stick->SetRawButton(6, false);
	Sleep(50);
	ASSERT_TRUE(button.Get());
	stick->SetRawButton(6, true);
	Sleep(50);
	ASSERT_FALSE(button.Get());
	stick->SetRawButton(6, false);
}

