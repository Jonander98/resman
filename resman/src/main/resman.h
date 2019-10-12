#pragma once

#include "pch.h"
#include "resource.h"
#include "data_structures/texture.h"

using resource_name = str_t;
using resource_type = texture;
class resman
{
public:
  ~resman();
  resman() = default;
  resman(const resman &) = delete;
  resman & operator=(const resman &) = delete;
public:
  resource_ptr<resource_type> get(const resource_name &)const;
private:
  std::map<resource_name, resource_type> m_data;
};

#include "src/resman.cpp"