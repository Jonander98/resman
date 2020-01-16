/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "usage/model.h"
#include "utils/filepath.h"

file_path model::get_file_path() const
{
  return m_file;
}

bool model::load(const file_path & path)
{
  return true;
}