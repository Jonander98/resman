/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "main/work_group.h"

#pragma region worker

namespace work_scheduling
{
#ifdef _DEBUG
#define XWORKERMSG(...) XMESSAGE("[Worker ", m_worker_id, "]", __VA_ARGS__)
  u32 worker::s_worker_ids = 0;
#else
#define XWORKERMSG(...)
#endif // _DEBUG

  worker::worker(shared_writeable_worker_data& wr, const shared_readable_worker_data& rd, shared_worker_mutex_data& m)
    : m_mutex(m)
    , m_read(rd)
    , m_write(wr)
    , m_closed(true)
  {
#ifdef _DEBUG
    m_worker_id = s_worker_ids++;
#endif // _DEBUG
  }

  worker::~worker()
  {
    start_closing();
    //Wait for the task to end
    if (m_thread.joinable())
      m_thread.join();
    XWORKERMSG("Worker Destroyed");
  }

  worker::worker(worker&& rhs)
    : m_thread(std::move(rhs.m_thread))
    , m_closed(rhs.m_closed.load())
    , m_mutex(rhs.m_mutex)
    , m_read(rhs.m_read)
    , m_write(rhs.m_write)
  {
  }

  void worker::activate()
  {
    XASSERTMSG(m_closed, "Activating a worker that is already active");
    if (XWASTECHECKCODE_TRUE(m_closed))
    {
      if (m_thread.joinable())
        m_thread.join();
      m_closed = false;
      m_thread = std::thread(&worker::thread_loop, this);
    }
  }

  void worker::start_closing()
  {
    XWORKERMSG("Asking worker to close");
    m_closed = true;
    m_mutex.m_task_added_condition.notify_all();
  }

  bool worker::is_active()const
  {
    return !m_closed;
  }

  bool worker::is_task_available() const
  {
    return !m_read.m_task_groups.empty() && m_write.m_task_idx < m_read.m_task_groups[m_read.m_task_group_idx].size();
  }

  task worker::try_get_next_task()
  {
    XWORKERMSG("Worker trying to get a task");
    //This will allow workers to not block betweent them, but it will block when cleaning/moving is being made
    std::shared_lock<std::shared_mutex> slock(m_mutex.m_task_group_changing_mutex);
    //The idea is that while we are changing task groups this will be locked by the unique lock where we change it and then if we are unlocked
    //for the task group we also have to wait until a task is available. As the wait releases the lock this shouldn't cause deadlock for the thread
    //advancing the task group
    //NOTE: If we change to use notify_all when adding a task instead of notify_one there exists the risk that multiple threads bypass the check of them having
    //a task when there is only 1 available and skip this wait, so it is important to cover that case below by returning an empty task
    XWORKERMSG("Worker about to wait");
    m_mutex.m_task_added_condition.wait(slock, [this]() 
    {
      bool const task_available = is_task_available();
      if (!task_available)
      {
        m_mutex.m_worker_idle.notify_one();
      }
      return !is_active() || task_available;
    });
    XWORKERMSG((is_active() ? "Worker found a new task" : "Worker exiting because of deactivation"));
    if (!is_active())
    {
      return task();
    }
    //Gather the group index safely
    u32 local_group_idx = m_read.m_task_group_idx;
    XASSERTMSG(local_group_idx < m_read.m_task_groups.size(), "Invalid task group with index ", local_group_idx, " when the number of task groups was ", m_read.m_task_groups.size());
    if (XWASTECHECKCODE_FALSE(local_group_idx >= m_read.m_task_groups.size()))
      return task();

    //Gather the task index safely
    u32 local_idx = m_write.m_task_idx++;
    if (local_idx >= m_read.m_task_groups[local_group_idx].size())
    {
      //If we enter here it should only be because we are using notify_all for the condition above and
      //multiple threads saw that the same task was available, so we return an empty task to the threads that missed it.
      return task();
    }
    //Check if this was the last task
    if (local_idx + 1 == m_read.m_task_groups[local_group_idx].size())
    {
      XMESSAGE("Notifying advance group");
      //Let the advancer thread know that we should move to the next task group
      m_mutex.m_end_of_task_group_reached_condition.notify_one();
    }
    return m_read.m_task_groups[local_group_idx][local_idx];
  }

  void worker::thread_loop()
  {
    //Fetch tasks until we tell the worker to close
    while (is_active())
    {
      task next_task = try_get_next_task();

      if (next_task != nullptr)
      {
        XWORKERMSG("Executing task");
        //call the load
        next_task();
        m_write.m_num_remaining_tasks--;
        XWORKERMSG("Finished task");
      }
    }
  }

}
#pragma endregion