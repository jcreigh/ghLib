/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/
#ifndef SRC_GHLIB_DIGITALINPUT_H_
#define SRC_GHLIB_DIGITALINPUT_H_

#ifndef NOROBOT

#include "DigitalInput.h"
namespace ghLib {
typedef ::frc::DigitalInput DigitalInput;
}

#else

#include <unordered_map>

namespace ghLib {

class DigitalInput {
public:
	DigitalInput(int channel);
	bool Get() const;
	void Set(bool newValue);

private:
	int channel;
	static std::unordered_map<int, bool> values;

};

}

#endif

#endif /* SRC_GHLIB_DIGITALINPUT_H_ */
