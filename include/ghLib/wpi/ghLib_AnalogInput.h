/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2017. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef SRC_GHLIB_ANALOGINPUT_H_
#define SRC_GHLIB_ANALOGINPUT_H_

#ifndef NOROBOT

#include "AnalogInput.h"
namespace ghLib {
typedef ::frc::AnalogInput AnalogInput;
}

#else

#include <unordered_map>

namespace ghLib {

class AnalogInput {
public:
	AnalogInput(int channel);
	int GetValue();
	int GetAverageValue();
	void SetValue(int newValue);

private:
	int channel;
	static std::unordered_map<int, int> values;

};

}

#endif

#endif /* SRC_GHLIB_ANALOGINPUT_H_ */
