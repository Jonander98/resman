#pragma once

namespace
{
  template<typename ...Args>
  void log(std::ostream& out, Args && ...args)
  {
    (out << ... << args);
    out << std::endl;
  }
}

void assert(bool condition, const char* condition_str, const char* location, const char * fmt = nullptr, ...);
void print(std::ostream& out = std::cout, const char* fmt = nullptr, ...);

// Macros
#if _WIN32
#define XBREAK() __debugbreak()
#else
#define XBREAK() __builtin_trap()
#endif

#define __STRINGIZE__1(x) #x
#define __STRINGIZE__2(x) __STRINGIZE__1(x)
#define __LOCATION__ __FILE__ " : " __STRINGIZE__2(__LINE__)



#define XASSERT(what) assert(what, #what, __LOCATION__)
#define XASSERTMSG(what, fmt, ...) assert(what, #what, __LOCATION__, fmt, __VA_ARGS__)
#define XERROR(what) XASSERT(false)

#ifdef _DEBUG
#define ENABLE_PRINTS

#define XWASTECHECKCODE_TRUE(what) what
#define XWASTECHECKCODE_FALSE(what) what
#ifdef ENABLE_PRINTS
#define XMESSAGE(fmt, ...) print(std::cout, fmt, __VA_ARGS__);
#else
#define XMESSAGE(...) (void) __VA_ARGS__
#endif // ENABLE_PRINTS

#else
//#define XASSERT(what)
//#define XASSERTMSG(what, ...)
//#define XERROR(what)
#define XWASTECHECKCODE_TRUE(what) true
#define XWASTECHECKCODE_FALSE(what) false
#define XMESSAGE(...)
#endif // _DEBUG