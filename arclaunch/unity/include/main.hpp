#ifndef INCLUDE_MAIN_HPP_
#define INCLUDE_MAIN_HPP_

#ifdef WIN32
#define AC_API extern "C" __declspec(dllexport)
#else
#define AC_API extern "C"
#endif

#endif /* INCLUDE_MAIN_HPP_ */