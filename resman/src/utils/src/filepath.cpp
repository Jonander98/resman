/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "utils/filepath.h"

Filepath::Filepath(const str_t & path)
  : m_fullpath(path)
{
  const size_t nullendpos = m_fullpath.find_first_of('\0');
  if (nullendpos != str_t::npos)
  {//We have null chars at the end
    m_fullpath.erase(std::next(m_fullpath.begin(), nullendpos), m_fullpath.end());
  }



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
  {
    m_name = m_fullpath;
    return;//Invalid path
  }
  
  m_extension = m_fullpath.substr(dot++);

  size_t slash = m_fullpath.find_last_of('/');
  if (slash == str_t::npos)
    slash = 0;
  else
    ++slash;//Ignore the slash

  m_name = m_fullpath.substr(slash, --dot - slash);

}

Filepath::Filepath(const char * path)
  : Filepath(str_t(path))
{}

const str_t & Filepath::GetFullPath() const
{
  return m_fullpath;
}

const str_t & Filepath::GetExtension() const
{
  return m_extension;
}

const str_t & Filepath::GetName() const
{
  return m_name;
}

const str_t Filepath::GetFullName() const
{
  return m_name + m_extension;
}
