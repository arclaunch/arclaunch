#ifndef INCLUDE_UNITS_HPP_
#define INCLUDE_UNITS_HPP_

float operator""_uc(long double inValue) { return float(inValue * pow(10, -6)); };
float operator""_ukg(long double inValue) { return float(inValue * pow(10, -6)); };

#endif /* INCLUDE_UNITS_HPP_ */