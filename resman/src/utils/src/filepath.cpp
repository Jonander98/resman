

#include "pch.h"
#include "utils/filepath.h"

file_path::file_path(const str_t & path)
  : m_fullpath(path)
{
  //Normalize the path
  while(true)
  {
    size_t pos = m_fullpath.find_first_of('\\');
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

file_path::file_path(const char * path)
  : file_path(str_t(path))
{}

const str_t & file_path::get_fullpath() const
{
  return m_fullpath;
}

const str_t & file_path::get_extension() const
{
  return m_extension;
}

const str_t & file_path::get_name() const
{
  return m_name;
}

const str_t file_path::get_full_name() const
{
  return m_name + m_extension;
}
