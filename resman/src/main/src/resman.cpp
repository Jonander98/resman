/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resman.h"

resman::~resman()
{
  //unload_all();
  //Clean all the registered resource containers
  for (auto pair : m_resources)
    delete pair.second;
}
//TODO: Find another way to do this as it is pretty uggly
class dummy_resource : public resource { bool load(const file_path &)override { return false; } };
void resman::unload_all()
{
  //for (auto & ct_pair : m_resources)
  //{
  //  auto ct = reinterpret_cast<resource_container<dummy_resource>*>(ct_pair.second);
  //  for (auto & pair : *ct)
  //    pair.second.m_resource.internal_unload();
  //  ct->clear();
  //}
}

void resman::set_log(const message_log & log)
 {
   m_log = log;
 }

 const message_log & resman::get_log() const
 {
   return m_log;
 }
