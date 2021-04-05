/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"


class Model : public AResource
{
private:
  bool VLoad(const Filepath &)override final;
  void VUnload()override;
  i32 m_foo;
  std::vector<u32> m_bar;
  std::array<i32, 80> m_mar;
};