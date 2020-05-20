/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "main/work_group.h"

#pragma region worker



work_group::task work_group::request_next_task_ownership()
{
  //This will allow workers to not block betweent them, but it will block when cleaning is being made
  std::shared_lock<std::shared_mutex> slock(m_cleaning_mutex);
  if (m_task_groups.empty())
    return task();
  //Gather the group index safely
  u32 local_group_idx = m_task_group_idx;
  if (m_task_group_idx >= m_task_groups.size())
    return task();

  //Gather the task index safely
  u32 local_idx = m_task_idx++;
  if (local_idx >= m_task_groups[local_group_idx].size())
  {
    local_group_idx = ++m_task_group_idx;
    if (local_group_idx >= m_task_groups.size())
      return task();

    m_task_idx = 1;
    local_idx = 0;
  }
  m_num_remaining_tasks--;
  return m_task_groups[local_group_idx][local_idx];
}

void work_group::check_if_extra_worker_is_needed()
{
  if (m_workers.empty())
  {//Check if it is the first one
    if (m_num_remaining_tasks != 0)
    {
      m_workers.emplace_back(std::move(worker(*this)));
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
    size_t tasks_per_thread = m_num_remaining_tasks / --num_workers;
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

    size_t tasks_per_thread = m_num_remaining_tasks / m_workers.size();

    //Check if we should create a new worker
    if (tasks_per_thread > m_config.min_resources_to_fork && m_config.max_threads > m_workers.size())
      m_workers.emplace_back(worker(*this));
    else
      break;
  }
}

void work_group::clean_finished_task()
{
  if (m_task_group_idx == 0)
    return;
  std::lock_guard<std::shared_mutex> lock(m_cleaning_mutex);

  m_task_groups.erase(m_task_groups.begin(), std::next(m_task_groups.begin(), m_task_group_idx-1));
  m_task_group_idx = 0;
  m_num_total_tasks = 0;
  for (const auto& cont : m_task_groups)
    m_num_total_tasks += cont.size();
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
  add_task(std::vector<task>{t});
}

void work_group::add_task(std::vector<task> tasks)
{
  if (tasks.empty())
    return;
  const u32 MAX_NUM_FINISHED_STORED_TASKS = 0;
  if (m_num_total_tasks - m_num_remaining_tasks > MAX_NUM_FINISHED_STORED_TASKS)
    clean_finished_task();

  m_task_groups.push_back(tasks);
  m_num_remaining_tasks += tasks.size();
  m_num_total_tasks += tasks.size();
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
  while (!m_closed)
  {
    //Get the tasks safely
    task cur = m_work_group.request_next_task_ownership();

    if (cur != nullptr)
    {
      //call the load
      cur();
    }
    else
    {
      //Not tasks remaining
      m_closed = true;
    }
    
    
  }
}

#pragma endregion