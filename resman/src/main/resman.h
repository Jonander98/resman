/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "resource.h"
#include "usage/texture.h"

using resource_name = str_t;
using resource_type = texture;
class resman
{
public:
  resman() = default;
  resman(const resman &) = delete;
  resman & operator=(const resman &) = delete;
public:
  //If the id is registered, a valid pointer to a resource is given. But it might be not loaded yet
  //If the id is not registered, null is returned
  resource_ptr<resource_type> get(const resource_name &)const;
private:
  std::map<resource_name, resource_type> m_data;
};

#include "src/resman.inl"