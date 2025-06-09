#ifndef INCLUDE_UNITS_HPP_
#define INCLUDE_UNITS_HPP_

constexpr float operator""_uc(long double inValue) { return float(inValue * pow(10, -6)); };
constexpr float operator""_ukg(long double inValue) { return float(inValue * pow(10, -6)); };

#endif /* INCLUDE_UNITS_HPP_ */