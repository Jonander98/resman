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
    throw "No more tasks";
  i32 local_idx = m_task_idx++;
  if (local_idx >= m_task_groups[m_task_group_idx].size())
  {
    m_task_group_idx++;
    if (m_task_group_idx >= m_task_groups.size())
      throw "No more tasks";

    m_task_idx = 1;
    local_idx = 0;
  }
  m_num_tasks--;
  return m_task_groups[m_task_group_idx][local_idx];
}

void work_group::check_if_extra_worker_is_needed()
{
  if (m_workers.empty())
  {//Check if it is the first one
    if (m_num_tasks != 0)
    {
      m_workers.emplace_back(worker(*this));
      m_workers.back().activate();
    }
    return;
  }

  size_t num_workers = m_workers.size();

  for (auto it = m_workers.begin(); it != m_workers.end();)
  {
    if (it->is_active())
    {
      ++it;
      continue;
    }
    //Decide if we want to close the inactive worker or reactivate it

    //Simulate we have one less to check if we need it
    size_t tasks_per_thread = m_num_tasks / --num_workers;
    if (tasks_per_thread > m_config.min_resources_to_fork && m_config.max_threads > num_workers)
    {
      it->activate();
      ++num_workers;
      ++it;
      //Not breaking here to activate all the inactive workers, but we are not removing any
    }
    else
    {
      it = m_workers.erase(it);
    }
  }

  while (true)
  {

    size_t tasks_per_thread = m_num_tasks / num_workers;

    //Check if we should create a new worker
    if (tasks_per_thread > m_config.min_resources_to_fork && m_config.max_threads > num_workers)
      m_workers.emplace_back(worker(*this));
    else
      break;
  }
}

work_group::work_group(config cf)
  : m_config(cf)
{}

work_group::~work_group()
{
  //Wait until the last task is done
  m_workers.clear();
}

void work_group::add_task(task t)
{
  m_task_groups.push_back(std::vector<task>{t});
  m_num_tasks++;
  check_if_extra_worker_is_needed();
}

void work_group::add_task(std::vector<task> tasks)
{
  if (tasks.empty())
    return;
  m_task_groups.push_back(tasks);
  m_num_tasks += tasks.size();
  check_if_extra_worker_is_needed();
}

void work_group::stop_execution()
{
  m_workers.clear();
}




work_group::worker::worker(work_group& gp)
  : m_work_group(gp), m_closed(true)
{
}

work_group::worker::~worker()
{
  //Wait for the task to end
  if (m_thread.joinable())
    m_thread.join();
}

work_group::worker::worker(worker&& rhs)
  : m_thread(std::move(rhs.m_thread)), m_closed(rhs.m_closed.load()), m_work_group(rhs.m_work_group)
{
}

void work_group::worker::activate()
{
  if (m_closed)
  {
    if(m_thread.joinable())
      m_thread.join();
    m_closed = false;
    m_thread = std::thread(&worker::thread_loop, this);
  }
}

bool work_group::worker::is_active()
{
  return !m_closed;
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
    catch (const char *)
    {//Not tasks remaining
      m_closed = true;
      return;
    }
  }
}

#pragma endregion