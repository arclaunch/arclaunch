#include "units.hpp"
#include <cmath>

float operator""_uc(long double inValue) { return float(inValue * pow(10, -6)); };
float operator""_ukg(long double inValue) { return float(inValue * pow(10, -6)); };