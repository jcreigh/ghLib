/*----------------------------------------------------------------------------*/
/* Copyright (c) Creighton 2015. All Rights Reserved.                         */
/* Open Source Software - May be modified and shared but must                 */
/* be accompanied by the license file in the root source directory            */
/*----------------------------------------------------------------------------*/

#ifndef GHLIB_UTIL_H_
#define GHLIB_UTIL_H_

#include <memory>
#include <vector>
#include <sstream>
#include <stdarg.h>
#include <string.h>
#include <algorithm>
#include "ghLib/util/Clock.h"
#include "ghLib/util/Filesystem.h"

namespace ghLib {

// C++ templates are some witchcraft
// https://en.wikipedia.org/wiki/Template_%28C%2B%2B%29#Function_templates
// They let you define a generic function which operates on type T
// Below, coerce(-2.0, -1.0, 1.0) operates on floats (well, doubles). When the compiler
// sees this, it takes the template below and basically replaces all the T's
// with "double". When it sees coerce(1, 5, 10), it replaces all the T's with "int"
template<class T>
T Coerce(T v, T r_min, T r_max) {
	// Coerces v into [r_min, r_max]
	// i.e. if v < r_min, return r_min
	//      if v > r_max, return r_max
	//      else,         return v
	return std::min(r_max, std::max(r_min, v));
}

// A bit more complex, using 2 types instead of just one.
template<class T, class U>
U Interpolate(T v, T d_min, T d_max, U r_min, U r_max) {
	// Coerce v into [d_min, d_max]
	// then linear interpolates into [r_min, r_max]
	// https://en.wikipedia.org/wiki/Linear_interpolation
	// (y - y0) / (x - x0) = (y1 - y0) / (x1 - x0)
	// y = y0 + (y1 - y0) * (x - x0) / (x1 - x0)
	// y = r_min + (r_max - r_min) * (v - d_min) / (d_max - d_min)
	// y = (v - d_min) * (r_max - r_min) / (d_max - d_min) + r_min
	return static_cast<U>((Coerce(v, d_min, d_max) - d_min) * (r_max - r_min) / (d_max - d_min)) + r_min;
}

template<class T>
T Deadband(T v, T deadband) {
	// If v is within `deadband` of 0, then just return 0.
	// Else, interpolate |v| from [deadband, 1] into [0, 1] and
	// make it negative if it was that before
	return (std::abs(v) < deadband) ? 0 : (std::signbit(v) ? -1 : 1) * Interpolate<T, T>(std::abs(v), deadband, 1.0, 0.0, 1.0);
}

std::string Format(const std::string fmt_str, ...);
std::string FilterASCII(const std::string in);
std::string EscapeString(const std::string in);
std::vector<std::string>& Tokenize(const std::string &input, char delimeter, std::vector<std::string>& tokens, char escape = '\0');
std::vector<std::string> Tokenize(const std::string &input, char delimeter, char escape = '\0');

}

#endif /* GHLIB_UTIL_H_ */
