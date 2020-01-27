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
  for (auto & w : m_workers)
    w.close_thread();

  m_workers.clear();
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

 resman::worker & resman::find_best_worker()
 {
   size_t best_idx = 0;
   size_t num_tasks = std::numeric_limits<size_t>::max();
   for (size_t i = 0; i < m_workers.size(); ++i)
   {
     if (size_t n = m_workers[i].get_task_queue_size() < num_tasks)
     {
       best_idx = i;
       num_tasks = n;
     }
   }
   return m_workers[best_idx];
 }

#pragma region worker
 resman::worker::~worker()
 {
   close_thread();
   if (m_thread.joinable())
     m_thread.join();
 }
 void resman::worker::add_task(task fn)
 {
   if (m_thread.joinable())
   {
     if (m_should_stop == true)
     {//Close and create
       m_thread.join();
       m_thread = std::thread(&worker::thread_loop, this);
     }
   }
   else
   {//Create one
     m_thread = std::thread(&worker::thread_loop, this);
   }
   std::lock_guard<std::mutex> lock(m_task_mutex);
   m_tasks.push(fn);
 }

 size_t resman::worker::get_task_queue_size() const
 {
   return m_tasks.size();
 }

 void resman::worker::close_thread()
 {
   m_should_stop = true;
 }

 void resman::worker::thread_loop()
 {
   while (m_should_stop == false)
   {
     if (m_tasks.empty())
       continue;
     task cur;
     {
       std::lock_guard<std::mutex> lock(m_task_mutex);
       cur = m_tasks.front();
     }
     //call the load
     (cur.who->*(cur.what))(cur.how);
     std::lock_guard<std::mutex> lock(m_task_mutex);
     m_tasks.pop();
   }
 }
#pragma endregion