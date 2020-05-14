/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once

//Containers
#include <string>
#include <vector>
#include <map>
#include <array>
#include <queue>
#include <list>
//IO
#include <iostream>
//Others
#include <thread>
#include <typeinfo>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <functional>

using f64 = double;
using f32 = float;
using i64 = int64_t;
using i32 = int32_t;
using i16 = int16_t;
using i8 = int8_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

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