/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#include "ghLib/wpi/DigitalInput.h"

#ifdef NOROBOT
namespace ghLib {

std::unordered_map<int, bool> DigitalInput::values;

DigitalInput::DigitalInput(int channel) : channel(channel) {
	values[channel] = false;
}

bool DigitalInput::Get() const {
	return values[channel];
}

void DigitalInput::Set(bool newValue) {
	values[channel] = newValue;
}

}
#endif
