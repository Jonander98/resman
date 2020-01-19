/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"


class model : public resource
{
private:
  bool load(const file_path &)override final;
  void unload()override;
  i32 foo;
  std::vector<u32> bar;
  std::array<i32, 80> mar;
};