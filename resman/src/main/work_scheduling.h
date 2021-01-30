#pragma once

#include "pch.h"

namespace work_scheduling
{
  using task = std::function<void(void)>;
  using task_group = std::queue<task>;
}