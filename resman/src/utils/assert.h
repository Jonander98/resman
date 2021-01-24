#pragma once

namespace
{
  template<typename ...Args>
  void log(std::ostream & out, Args && ...args)
  {
    (out << ... << args);
    out << std::endl;
  }
}
void assert(bool condition, const char* condition_str, const char* location, ...);
void print(...);

// Macros
#if _WIN32
#define XBREAK() __debugbreak()
#else
#define XBREAK() __builtin_trap()
#endif

#define __STRINGIZE__1(x) #x
#define __STRINGIZE__2(x) __STRINGIZE__1(x)
#define __LOCATION__ __FILE__ " : " __STRINGIZE__2(__LINE__)
#ifdef _DEBUG

#define XASSERT(what) assert(what, #what, __LOCATION__)
#define XASSERTMSG(what, ...) assert(what, #what, __LOCATION__, __VA_ARGS__)
#define XERROR(what) {std::cerr << "Error: " << what; XBREAK(); throw std::runtime_error("Error at " __LOCATION__); }
#define XWASTECHECKCODE_TRUE(what) what
#define XWASTECHECKCODE_FALSE(what) what
#define XMESSAGE(...) log(std::cout, __VA_ARGS__)

#else
#define XASSERT(what)
#define XASSERTMSG(what, ...)
#define XERROR(what)
#define XWASTECHECKCODE_TRUE(what) true
#define XWASTECHECKCODE_FALSE(what) false
#define XMESSAGE(...) (void) __VA_ARGS__
#endif // _DEBUG