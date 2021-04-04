
#include "pch.h"
#include "utils/assert.h"

#pragma warning(push)
#pragma warning (disable:4100) //unreferenced formal parameter

void print(std::ostream& out, const char* fmt, va_list args)
{
#ifdef _ENABLE_PRINTS

  static std::mutex printMutex;
  std::lock_guard<std::mutex> lock(printMutex);
  if (fmt)
  {
    vprintf(fmt, args);
    printf("\n");
  }
  else
  {
    log(out, args);
    std::cout << std::endl;
  }

#endif // _ENABLE_PRINTS
}

void print(std::ostream& out, const char * fmt, ...)
{
#ifdef _ENABLE_PRINTS
  std::va_list args;
  va_start(args, fmt);
  print(out, fmt, args);
  va_end(args);
#endif // _ENABLE_PRINTS
}

void assert(bool condition, const char* condition_str, const char* location, const char * fmt, ...)
{
  if (!condition)
  {
    print(std::cerr, "Assert failed: %s", condition_str);
    std::va_list args;
    va_start(args, location);
    print(std::cerr, fmt, args);
    va_end(args);
    XBREAK();
    throw std::runtime_error(std::string("Assert failed at ") + location);
  }
}

#pragma warning(pop)