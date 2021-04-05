/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"

class Filepath
{
public:
  /**
  * @brief  Creates a filepath from string
  * @param  the path
  */
  Filepath(const str_t & path);
  Filepath(const char * path = "");
  /**
  * @brief  Returns the full path
  * @return the full path
  */
  const str_t & GetFullPath()const;
  /**
  * @brief  Returns the extension of the file
  * @return the extension
  */
  const str_t & GetExtension()const;
  /**
  * @brief  Returns the name of the file
  * @return the name
  */
  const str_t & GetName()const;
  /**
  * @brief  Returns the name of the file with the extension
  * @return the name with the extension
  */
  const str_t GetFullName()const;

private:
  str_t m_fullpath;  //The whole path
  str_t m_extension; //The extension
  str_t m_name;      //The name
};