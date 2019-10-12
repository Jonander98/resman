
#pragma once

#include "pch.h"
#include "main/resource.h"

class file_path;

class texture : public resource
{
public:
  /**
  * @brief Loads the texture from the given path
  * @param The path of the texture
  * @return True if the load was successfull
  */
  bool load(const file_path & path);
};