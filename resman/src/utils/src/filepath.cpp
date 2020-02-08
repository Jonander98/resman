/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "utils/filepath.h"

filepath::filepath(const str_t & path)
  : m_fullpath(path)
{
  size_t pos = 0;
  //Normalize the path
  while(true)
  {
    pos = m_fullpath.find_first_of('\\', pos);
    if (pos == str_t::npos)
      break;
    m_fullpath[pos] = '/';
  }

  size_t dot = m_fullpath.find_last_of('.');
  if (dot == str_t::npos)
    return;//Invalid path
  
  m_extension = m_fullpath.substr(dot++);

  size_t slash = m_fullpath.find_last_of('/');
  if (slash == str_t::npos)
    slash = 0;
  else
    ++slash;//Ignore the slash

  m_name = m_fullpath.substr(slash, --dot - slash);

}

filepath::filepath(const char * path)
  : filepath(str_t(path))
{}

const str_t & filepath::get_fullpath() const
{
  return m_fullpath;
}

const str_t & filepath::get_extension() const
{
  return m_extension;
}

const str_t & filepath::get_name() const
{
  return m_name;
}

const str_t filepath::get_full_name() const
{
  return m_name + m_extension;
}
