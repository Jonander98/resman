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
//IO
#include <iostream>
#include <fstream>
#include <sstream>
//Others
#include <thread>
#include <typeinfo>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <algorithm>
#include <cstdarg>

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


#include "utils/assert.h"