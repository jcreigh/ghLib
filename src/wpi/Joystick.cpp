/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/wpi/ghLib_Joystick.h"

#ifdef NOROBOT
namespace ghLib {

std::unordered_map<int, Joystick*> Joystick::sticks_;

Joystick::Joystick() {
	SetAxisCount(6);
	SetButtonCount(12);
	SetPOVCount(1);
}

int Joystick::GetButtonCount() {
	return buttonCount;
}

void Joystick::SetButtonCount(int newCount) {
	buttons.resize(newCount);
	buttonCount = newCount;
}

int Joystick::GetRawButton(uint32_t button) {
	return buttons[button - 1];
}

void Joystick::SetRawButton(uint32_t button, bool newValue) {
	buttons[button - 1] = newValue;
}


int Joystick::GetAxisCount() {
	return axisCount;
}

void Joystick::SetAxisCount(int newCount) {
	axes.resize(newCount);
	axisCount = newCount;
}

float Joystick::GetRawAxis(uint32_t axis) {
	return axes[axis];
}

void Joystick::SetRawAxis(uint32_t axis, float newValue) {
	axes[axis] = newValue;
}


int Joystick::GetPOVCount() {
	return povCount;
}

void Joystick::SetPOVCount(int newCount) {
	povs.resize(newCount);
	povCount = newCount;
}

int Joystick::GetPOV(uint32_t pov) {
	return povs[pov];
}

void Joystick::SetPOV(uint32_t pov, int newValue) {
	povs[pov] = newValue;
}


Joystick* Joystick::GetStickForPort(int stickNum) {
	if (sticks_.count(stickNum) == 0) {
		sticks_[stickNum] = new Joystick();
	}
	return sticks_[stickNum];
}

}
#endif
