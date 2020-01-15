/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resource.h"

bool resource::is_loaded() const
{
  return m_is_loaded;
}

void resource::internal_load(const file_path & fp)
{
  load(fp);
  m_is_loaded = true;
}
