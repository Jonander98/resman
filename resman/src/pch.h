/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once

//Containers
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <array>
//IO
#include <iostream>
//Others
#include <thread>
#include <typeinfo>

using f64 = double;
using f32 = float;
using i64 = long;
using i32 = int;
using i16 = short;
using i8 = char;
using u64 = unsigned long;
using u32 = unsigned;
using u16 = unsigned short;
using u8 = unsigned char;

using str_t = std::string;



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