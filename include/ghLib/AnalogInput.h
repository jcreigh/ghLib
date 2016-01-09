/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/
#ifdef ROBOT

#include "WPILib.h"
namespace ghLib {
typedef ::AnalogInput AnalogInput;
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
