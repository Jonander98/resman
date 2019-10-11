/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written consent of
DigiPen Institute of Technology is prohibited.
Purpose: Helpers for handling errors
Author: Jon Ander Jimenez, jonander.jimenez, 540001716
- End Header --------------------------------------------------------*/
#pragma once


#include <iostream> //std::cerr



// Macros
#if _WIN32
#define XBREAK() __debugbreak()
#else
#define XBREAK() __builtin_trap()
#endif

#define __STRINGIZE__1(x) #x
#define __STRINGIZE__2(x) __STRINGIZE__1(x)
#define __LOCATION__ __FILE__ " : " __STRINGIZE__2(__LINE__)
#define XASSERT(what) if(!(what)){ std::cerr << "Assert failed: "#what; XBREAK(); throw std::runtime_error("Assert failed at " __LOCATION__);}
#define XERROR(what) {std::cerr << "Error: " << what; XBREAK(); throw std::runtime_error("Error at " __LOCATION__); }


