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

resource::id_type resource::get_id() const
{
  return m_id;
}

void resource::unload()
{
  //Nothing by default
}

void resource::internal_load(const filepath & fp)
{
  load(fp);
  m_is_loaded = true;
}

void resource::internal_unload()
{
  unload();
  m_is_loaded = false;
}
