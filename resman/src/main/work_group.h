/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/worker.h"
#include "main/work_scheduling.h"

namespace work_scheduling
{

  class work_group
  {
  public:
    struct config
    {
      //Maximum number of threads allowed
      u8 max_threads{ 4 };
      /*
      * Minimum number of resources that have to be in the queue of a thread to create a new one
      * always respecting the maximum
      */
      u8 min_resources_to_fork{ 3 };
    };

  private:

    struct shared_woker_data
    {
      //Know that workers might modify this
      shared_writeable_worker_data m_write;
      //Know that workers might be reading from this
      shared_readable_worker_data m_read;
      //Way to let workers know when to wait
      shared_worker_mutex_data m_mutex;
    };

  public:
    //Wait for the thread to finish execution
    work_group(config);
    ~work_group();
    //Adds a task to the queue
    void add_task(task fn);
    //Adds a vector of tasks to the queue
    void add_task(std::vector<task> tasks);
    //Waits for the threads to finish the current tasks and removes the workers
    void stop_execution();
  private:
    //Helper function to add a new worker
    void add_worker();
    //Checks if an extra worker is needed due to the extra load and creates one if necessary
    void activate_or_add_worker_if_needed();
    //Cleans the finished tasks from the vector. Should only be called when workers arent working
    void clean_finished_tasks();
    //Removes workers from the worker vector if they are not expected to be needed
    void erase_unnecesary_workers();
    //Starting function for the cleanup thread from the work_group
    void background_thread_loop();
    //Move to the next task group so that the workers can pick from it
    void advance_task_group_non_thread_safe();
  private:
    //Shared data between workers
    shared_woker_data m_shared;
    //The number of tasks that are stored
    size_t m_num_total_tasks{ 0 };
    //All the workers on the work group.
    std::vector<std::unique_ptr<worker>> m_workers;
    //Config related with when we should create a new worker
    config m_config;
    //Thread performing tasks in the background
    std::thread m_background_thread;
    //Flag to tell the background thread to stop
    std::atomic<bool> m_stop_background_thread;
  };
}
