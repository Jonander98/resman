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

resman::resman(config cf)
  : m_config(cf), m_work_group(work_group::config{cf.max_threads, cf.min_resources_to_fork})
{}


void resman::unload_all()
{
  for (auto pair : m_resources)
  {
    //Get the current container as a container of the parent resource class
    auto ct = reinterpret_cast<resource_container<resource>*>(pair.second);
    for (auto & in_pair : *ct)
    {
      //Call the unload trough the virtual table
      in_pair.second->internal_unload();
      delete in_pair.second.get();
    }
    ct->clear();
  }
}

void resman::set_config(config c)
{
  m_config = c;
}

void resman::set_log(const message_log & log)
 {
   m_log = log;
 }

 const message_log & resman::get_log() const
 {
   return m_log;
 }
 /*
 resman::work_group * resman::find_best_worker()
 {
   auto best_it = m_workers.begin();
   size_t min_num_tasks = std::numeric_limits<size_t>::max();
   //Get the worker with the minimum number of tasks
   for (auto it = m_workers.begin(); it != m_workers.end(); ++it)
   {
     size_t n = it->get_task_queue_size();
     if (n < min_num_tasks)
     {
       best_it = it;
       min_num_tasks = n;
     }
   }
   
   //Check if we should create a new worker
   if (min_num_tasks > m_config.min_resources_to_fork && m_config.max_threads > m_workers.size())
   {
     m_workers.emplace_front();
     best_it = m_workers.begin();
   }
   //Check if we don't have any worker
   if (best_it == m_workers.end())
   {
     m_log.error("Loading: Async load has been requested, but config does not allow thread creation");
     return nullptr;
   }
   return &(*best_it);
 }*/
