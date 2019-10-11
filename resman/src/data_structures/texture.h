
#pragma once

#include "pch.h"

class FilePath;

class Texture
{
public:
  /**
  * @brief Loads the texture from the given path
  * @param The path of the texture
  * @return True if the load was successfull
  */
  bool load(const FilePath & path);
};