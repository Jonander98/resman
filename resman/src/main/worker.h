#pragma once

#include "pch.h"
#include "main/work_scheduling.h"

namespace work_scheduling
{
  struct shared_writeable_worker_data
  {
    //The index of the task that needs to be gathered
    std::atomic<u32> m_task_idx{ 0 };
    //The number of tasks that are not done
    std::atomic<size_t> m_num_remaining_tasks{ 0 };
  };
  struct shared_readable_worker_data
  {
    //The index of the task group we are processing
    std::atomic<u32> m_task_group_idx{ 0 };
    //All the tasks that have been requested
    std::vector<std::vector<task>> m_task_groups;
  };
  struct shared_worker_mutex_data
  {
    //Mutex to block the workers from getting new tasks until the task group has advanced
    std::shared_mutex m_task_group_changing_mutex;
    //Condition that notifies when a new task has been added
    std::condition_variable_any m_task_added_condition;
    //Condition that checks if the end of a task group has been reached
    std::condition_variable_any m_end_of_task_group_reached_condition;
    //Condition that gets triggered when a worker is iddle
    std::condition_variable m_worker_idle;
  };

  class worker
  {
  public:
    worker(shared_writeable_worker_data&, const shared_readable_worker_data&, shared_worker_mutex_data&);
    //Join the thread
    ~worker();
    //No copy
    worker(const worker&) = delete;
    worker& operator=(const worker&) = delete;
    //Move constructor
    worker(worker&&);
    //Makes the worker start working again if it was closed
    void activate();
    //Makes the worker close when it finishes his current work
    void start_closing();
    //Returns true if the worker is loading something
    bool is_active()const;
  private:
    bool is_task_available() const;
    task try_get_next_task();
  private:
    //Function called by the thread to keep looking for tasks
    void thread_loop();
    //The thread used by the worker
    std::thread m_thread;
    //The shared data from the worker group
    shared_worker_mutex_data& m_mutex;
    shared_writeable_worker_data& m_write;
    const shared_readable_worker_data& m_read;
    //If the worker is closed
    std::atomic<bool> m_closed;
#ifdef _DEBUG
    static u32 s_worker_ids;
    u32 m_worker_id;
#endif // _DEBUG

  };

}