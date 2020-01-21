/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "main/resman.h"

resman::~resman()
{
  unload_all();
  //Clean all the registered resource containers
  for (auto pair : m_resources)
    delete pair.second;
  m_resources.clear();
}

void resman::unload_all()
{
  for (auto pair : m_resources)
  {
    auto ct = reinterpret_cast<resource_container<resource>*>(pair.second);
    for (auto & in_pair : *ct)
    {
      in_pair.second->internal_unload();
      delete in_pair.second.get();
    }
    ct->clear();
  }
}

void resman::set_log(const message_log & log)
 {
   m_log = log;
 }

 const message_log & resman::get_log() const
 {
   return m_log;
 }
