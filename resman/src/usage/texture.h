/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/resource.h"


class texture : public resource
{
public:
  file_path get_file_path()const;
private:
  bool load(const file_path &)override final;
private:
  file_path m_file;
};

/*
* Example of specifying the hash function for a resource
*/
template <>
struct resource_hash<texture>
{
  size_t operator() (const str_t & st)
  {
    //Use custom hashing here
    return std::hash<str_t>{}(st);
  }
};