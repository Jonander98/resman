/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "main/work_group.h"

#pragma region worker

namespace work_scheduling
{
  work_group::work_group(config cf)
    : m_config(cf)
    , m_background_thread(&work_group::background_thread_loop, this)
  {
  }

  work_group::~work_group()
  {
    stop_execution();
  }

  void work_group::add_task(task t)
  {
    //TODO: Merge short task vectors?
    XMESSAGE("Adding a task");
    add_task(std::vector<task>{t});
  }

  void work_group::add_task(std::vector<task> tasks)
  {
    if (tasks.empty())
      return;

    if (m_shared.m_read.m_task_groups.capacity() == m_shared.m_read.m_task_groups.size())
    {
      //We will resize, so we must lock just in case
      std::lock_guard lock(m_shared.m_mutex.m_task_group_changing_mutex);
      m_shared.m_read.m_task_groups.push_back(tasks);
    }
    else
    {
      m_shared.m_read.m_task_groups.push_back(tasks);
    }

    m_shared.m_write.m_num_remaining_tasks.fetch_add(tasks.size());
    m_num_total_tasks += tasks.size();
    activate_or_add_worker_if_needed();
  }

  void work_group::stop_execution()
  {
    m_workers.clear();
    m_stop_background_thread = true;
    m_shared.m_mutex.m_end_of_task_group_reached_condition.notify_one();
    if (m_background_thread.joinable())
    {
      m_background_thread.join();
    }
  }


  void work_group::add_worker()
  {
    XMESSAGE("New Worker Added");
    m_workers.emplace_back(std::make_unique<worker>(m_shared.m_write, m_shared.m_read, m_shared.m_mutex));
    m_workers.back()->activate();
  }

  void work_group::activate_or_add_worker_if_needed()
  {
    if (m_workers.empty())
    {//Check if it is the first one
      if (m_shared.m_write.m_num_remaining_tasks != 0)
      {
        add_worker();
      }
      return;
    }

    size_t num_active_workers = std::count_if(m_workers.begin(), m_workers.end(), [](const std::unique_ptr<worker>& w)
    {
      return w->is_active();
    });

    bool check_for_add = true;

    auto is_worker_needed = [this](size_t num_active_workers)
    {
      size_t tasks_per_thread = m_shared.m_write.m_num_remaining_tasks / num_active_workers;
      return tasks_per_thread > m_config.min_resources_to_fork && m_config.max_threads > num_active_workers;
    };

    for (auto it = m_workers.begin(); it != m_workers.end(); ++it)
    {
      const auto& worker_ptr = *it;
      if (!worker_ptr->is_active())
      {
        //Check if we should reactivate it
        if (is_worker_needed(num_active_workers))
        {
          //We should activate it as we need it
          worker_ptr->activate();
          ++num_active_workers;
        }
        else
        {
          //Don't add if we already saw we don't need more workers
          check_for_add = false;
          break;
        }
      }
    }
    //Add workers if needed
    if (check_for_add)
    {
      while (is_worker_needed(num_active_workers))
      {
        add_worker();
        ++num_active_workers;
      }
    }
  }

  void work_group::clean_finished_tasks()
  {
    //Critical section
    {
      if (m_shared.m_mutex.m_task_group_changing_mutex.try_lock())
      {
        //Checking for greater as it is possible that we cleaned up while a worker had a task in progress
        if (m_shared.m_write.m_num_remaining_tasks >= m_num_total_tasks)
        {//Repeat check as the status could have changed
          m_shared.m_mutex.m_task_group_changing_mutex.unlock();
          return;
        }
        XMESSAGE("Cleanup");
        auto& task_groups = m_shared.m_read.m_task_groups;
        const bool current_is_finished = m_shared.m_write.m_task_idx >= task_groups[m_shared.m_read.m_task_group_idx].size();
        //If we have finished the current, we can erase that one too
        const size_t index_modifier = current_is_finished ? 1 : 0;
        task_groups.erase(task_groups.begin(), std::next(task_groups.begin(), m_shared.m_read.m_task_group_idx + index_modifier));
        m_shared.m_read.m_task_group_idx = 0;
        m_shared.m_mutex.m_task_group_changing_mutex.unlock();
      }
    }
    //No longer changing task_groups
    m_num_total_tasks = 0;
    for (const auto& cont : m_shared.m_read.m_task_groups)
      m_num_total_tasks += cont.size();

  }

  void work_group::erase_unnecesary_workers()
  {
    //TODO FILL

  }

  void work_group::background_thread_loop()
  {
    while (!m_stop_background_thread)
    {
      //Check if we need to move task group.
      {
        std::unique_lock lock(m_shared.m_mutex.m_task_group_changing_mutex);
        constexpr std::chrono::seconds time_out(2);
        //Wait with timeout
        bool success = m_shared.m_mutex.m_end_of_task_group_reached_condition.wait_for(lock, time_out, [this]()
        {
          const auto& r = m_shared.m_read;
          //Save to read the task_index even if it can be changed by the workers as we are already locking them with the lock above.
          bool const task_available_in_current_group = !r.m_task_groups.empty() && m_shared.m_write.m_task_idx < r.m_task_groups[r.m_task_group_idx].size();
          bool const there_are_more_groups_available = r.m_task_groups.size() > r.m_task_group_idx + 1;
          return m_stop_background_thread || (!task_available_in_current_group && there_are_more_groups_available);
        });
        if (m_stop_background_thread)
          break;
        if (success)
        {
          XMESSAGE("Advancing task group");
          advance_task_group_non_thread_safe();
          m_shared.m_mutex.m_task_added_condition.notify_one();
        }
      }
      //Check if we need to cleanup old finished tasks
      clean_finished_tasks();
      //Check if we need to cleanup unused workers
      erase_unnecesary_workers();
    }

  }

  void work_group::advance_task_group_non_thread_safe()
  {
    //We have finished with the current group
    ++m_shared.m_read.m_task_group_idx;
    XMESSAGE("Incrementing group index to ", m_shared.m_read.m_task_group_idx.load());
    m_shared.m_write.m_task_idx.store(0);
  }


}

#pragma endregion