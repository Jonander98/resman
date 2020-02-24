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

 resman::worker * resman::find_best_worker()
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
 }

#pragma region worker
 resman::worker::~worker()
 {
   //Wait until the last task is done
   close_thread();
   if (m_thread.joinable())
     m_thread.join();
 }

 void resman::worker::add_task(task fn)
 {
   std::lock_guard<std::mutex> lock(m_task_mutex);
   m_tasks.push(fn);
   //Check if we already have a thread
   if (m_thread.joinable())
   {
     //Check if stop had been requested for that thread
     if (m_should_stop == true)
     {//Close and create a new one since it seems like we have to keep working
       //We wont just change the boolean since we might get into a race condition
       m_thread.join();
       m_should_stop = false;
       m_thread = std::thread(&worker::thread_loop, this);
     }
   }
   else
   {//Create one
     m_should_stop = false;
     m_thread = std::thread(&worker::thread_loop, this);
   }
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
   //Fetch tasks until we are asked to stop
   while (m_should_stop == false)
   {
     if (m_tasks.empty())
       continue;
     //Get the tasks safely
     task cur;
     {
       std::lock_guard<std::mutex> lock(m_task_mutex);
       cur = m_tasks.front();
     }
     //call the load
     (cur.who->*(cur.what))(cur.how);
     //Remove the task from the queue safely
     std::lock_guard<std::mutex> lock(m_task_mutex);
     m_tasks.pop();
   }
 }
#pragma endregion