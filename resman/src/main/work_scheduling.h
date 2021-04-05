#pragma once

#include "pch.h"

namespace WorkScheduling
{
  using Task = std::function<void(void)>;
  using TaskGroup = std::queue<Task>;
}