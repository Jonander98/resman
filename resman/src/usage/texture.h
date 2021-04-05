/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"


class Texture : public AResource
{
private:
  bool VLoad(const Filepath &)override;
  void VUnload()override;

  i32 foo;
  std::vector<u32> bar;
  std::array<i32, 80> mar;
};

/*
* Example of specifying the hash function for a resource
*/
template <>
struct ResourceHash<Texture>
{
  size_t operator() (const str_t & st)
  {
    //Use custom hashing here
    return std::hash<str_t>{}(st);
  }
};


