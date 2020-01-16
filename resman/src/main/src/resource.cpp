/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resource.h"


resource::resource(id_type id)
 : m_id(id), m_is_loaded(false)
{}

 bool resource::is_loaded() const
{
  return m_is_loaded;
}

 resource::id_type resource::get_id() const
 {
   return m_id;
 }

void resource::internal_load(const file_path & fp)
{
  load(fp);
  m_is_loaded = true;
}
