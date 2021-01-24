
#include "pch.h"
#include "utils/assert.h"

void assert(bool condition, const char* condition_str, const char* location, ...)
{
  if (!condition)
  {
    std::cerr << "Assert failed: " << condition_str;
    std::va_list args;
    log(std::cerr, args);
    XBREAK();
    throw std::runtime_error(std::string("Assert failed at ") + location);
  }
}

void print(...)
{
  std::va_list args;
  log(std::cout, args);
  std::cout << std::endl;
}
