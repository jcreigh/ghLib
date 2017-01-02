/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/
#ifndef SRC_GHLIB_JOYSTICK_H_
#define SRC_GHLIB_JOYSTICK_H_

#ifndef NOROBOT

#include "Joystick.h"
namespace ghLib {
typedef ::frc::Joystick Joystick;
}

#else

#include <vector>
#include <unordered_map>

namespace ghLib {

class Joystick {
public:
	Joystick();

	int GetButtonCount();
	void SetButtonCount(int newCount);
	int GetRawButton(uint32_t button);
	void SetRawButton(uint32_t button, bool newValue);

	int GetAxisCount();
	void SetAxisCount(int newCount);
	float GetRawAxis(uint32_t axis);
	void SetRawAxis(uint32_t axis, float newValue);

	int GetPOVCount();
	void SetPOVCount(int newCount);
	int GetPOV(uint32_t pov);
	void SetPOV(uint32_t pov, int newValue);

	static Joystick* GetStickForPort(int stickNum);

private:
	int buttonCount;
	int axisCount;
	int povCount;
	std::vector<bool> buttons;
	std::vector<float> axes;
	std::vector<int> povs;

	static std::unordered_map<int, Joystick*> sticks_;

};

}

#endif

#endif /* SRC_GHLIB_JOYSTICK_H_ */
