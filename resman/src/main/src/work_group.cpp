/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "main/work_group.h"

#pragma region worker



work_group::task work_group::request_next_task_ownership()
{
  if (m_task_groups.empty())
    throw;
  i32 local_idx = m_task_idx++;
  if (local_idx >= m_task_groups[m_task_group_idx].size())
  {
    m_task_group_idx++;
    if (m_task_group_idx >= m_task_groups.size())
      throw;//No more tasks

    m_task_idx = 1;
    local_idx = 0;
  }
  return m_task_groups[m_task_group_idx][local_idx];
}

work_group::work_group(config cf)
  : m_config(cf)
{}

work_group::~work_group()
{
  //Wait until the last task is done
  m_workers.clear();
}

void work_group::add_task(task fn)
{
  std::vector<task> vt;
  vt.push_back(fn);
  m_task_groups.push_back(vt);
}

void work_group::add_task(std::vector<task> tasks)
{
  if (tasks.empty())
    return;
  m_task_groups.push_back(tasks);
}



work_group::worker::worker(work_group& gp)
  : m_work_group(gp)
{
  m_thread = std::thread(&worker::thread_loop, this);
}

work_group::worker::~worker()
{
  //Wait for the task to end
  if (m_thread.joinable())
    m_thread.join();
}

void work_group::worker::activate()
{
  if (m_closed)
  {
    m_thread.join();
    m_closed = false;
    m_thread = std::thread(&worker::thread_loop, this);
  }
}

void work_group::worker::thread_loop()
{
  //Fetch tasks until we cant find new ones
  while (true)
  {
    try
    {
      //Get the tasks safely
      task cur = m_work_group.request_next_task_ownership();

      //call the load
      cur();

    }
    catch (...)
    {//Not tasks remaining
      m_closed = true;
      return;
    }
  }
}

#pragma endregion