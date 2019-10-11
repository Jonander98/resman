

#include "pch.h"
#include "utils/filepath.h"

FilePath::FilePath(const str_t & path)
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

FilePath::FilePath(const char * path)
  : FilePath(str_t(path))
{}

const str_t & FilePath::get_fullpath() const
{
  return m_fullpath;
}

const str_t & FilePath::get_extension() const
{
  return m_extension;
}

const str_t & FilePath::get_name() const
{
  return m_name;
}

const str_t FilePath::get_full_name() const
{
  return m_name + m_extension;
}
