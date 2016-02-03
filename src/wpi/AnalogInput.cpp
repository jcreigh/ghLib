/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/wpi/AnalogInput.h"

#ifdef NOROBOT
namespace ghLib {

std::unordered_map<int, int> AnalogInput::values;

AnalogInput::AnalogInput(int channel) : channel(channel) {
	values[channel] = 0;
}

int AnalogInput::GetValue() {
	return values[channel];
}

int AnalogInput::GetAverageValue() {
	return values[channel];
}

void AnalogInput::SetValue(int newValue) {
	values[channel] = newValue;
}

}
#endif
