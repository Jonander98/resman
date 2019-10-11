
#pragma once

#include "pch.h"

class FilePath
{
public:
  /**
  * @brief  Creates a filepath from string
  * @param  the path
  */
  FilePath(const str_t & path);
  FilePath(const char * path);
  /**
  * @brief  Returns the full path
  * @return the full path
  */
  const str_t & get_fullpath()const;
  /**
  * @brief  Returns the extension of the file
  * @return the extension
  */
  const str_t & get_extension()const;
  /**
  * @brief  Returns the name of the file
  * @return the name
  */
  const str_t & get_name()const;
  /**
  * @brief  Returns the name of the file with the extension
  * @return the name with the extension
  */
  const str_t get_full_name()const;

private:
  str_t m_fullpath;  //The whole path
  str_t m_extension; //The extension
  str_t m_name;      //The name
};