#pragma once

#include "pch.h"

namespace WorkScheduling
{
  using task = std::function<void(void)>;
  using task_group = std::queue<task>;
}