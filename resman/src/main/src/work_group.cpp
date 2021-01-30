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
  {
    {
      cleanup_data data
      {
         m_shared.m_task_groups,
         m_shared.m_task_group_idx
      };
      m_subroutines.m_single.m_cleanup = std::make_unique<cleanup_subroutine>(data, m_shared.m_task_group_changing_mutex);
    }
    {
      task_moving_data data
      {
         m_shared.m_input_tasks,
         m_shared.m_task_groups,
         m_shared.m_task_group_idx,
         *this
      };
      m_subroutines.m_single.m_task_moving = std::make_unique<task_moving_subroutine>(data, m_shared.m_task_group_changing_mutex, m_shared.m_input_task_mutex);
    }

    for (auto& sub : m_subroutines.m_all)
    {
      sub->start();
    }
  }

  work_group::~work_group()
  {
    XMESSAGE("Destroying Workgroup");
  }

  void work_group::add_task(task t)
  {
    XMESSAGE("Adding a task");
    add_task(std::vector<task>{t});
  }

  void work_group::add_task(std::vector<task> tasks)
  {
    if (tasks.empty())
      return;

    {
      std::lock_guard<std::mutex> lock(m_shared.m_input_task_mutex);
      m_shared.m_input_tasks.emplace_back(std::deque<task>(tasks.begin(), tasks.end()));
    }

    //TODO MERGE or SEPARATE

    m_subroutines.m_single.m_task_moving->notify_if_condition_met();
    m_num_remaining_tasks += tasks.size();
    activate_or_add_worker_if_needed();
  }

  void work_group::stop_execution()
  {
    for (auto& sub : m_subroutines.m_all)
    {
      sub->stop();
    }
    for (auto& worker : m_workers)
    {
      worker->stop();
    }
    m_workers.clear();
  }

  size_t work_group::get_num_remaining_tasks() const
  {
    return m_num_remaining_tasks;
  }

  void work_group::on_task_completed()
  {
    m_num_remaining_tasks--;
  }

  void work_group::on_task_group_end_reached()
  {
    //Update notification on selected subroutines
    m_subroutines.m_single.m_cleanup->notify_if_condition_met();
    //We might have not moved when adding a task because we already had many tasks in the shared pool,
    //So we notify here too
    m_subroutines.m_single.m_task_moving->notify_if_condition_met();
  }

  void work_group::on_task_moved_to_shared_pool(size_t num_moved_tasks)
  {
    //Notify workers
    std::vector<decltype(m_workers)::iterator> m_active_workers;
    m_active_workers.reserve(num_moved_tasks);
    for (auto it = m_workers.begin(); it != m_workers.end() && m_active_workers.size() != num_moved_tasks; ++it)
    {
      if ((*it)->is_active())
      {
        m_active_workers.push_back(it);
      }
    }
    for (auto& it : m_active_workers)
    {
      (*it)->notify_if_condition_met();
    }
  }

  void work_group::add_worker()
  {
    XMESSAGE("New Worker Added");

    worker_data data
    {
      m_shared.m_task_group_idx,
      m_shared.m_task_groups,
      *this
    };
    m_workers.emplace_back(std::make_unique<worker>(data, m_shared.m_task_group_changing_mutex));
    m_workers.back()->start();
  }

  void work_group::activate_or_add_worker_if_needed()
  {
    if (m_workers.empty())
    {//Check if it is the first one
      XASSERT(get_num_remaining_tasks() != 0);
      add_worker();
      return;
    }

    size_t num_active_workers = std::count_if(m_workers.begin(), m_workers.end(), [](const std::unique_ptr<worker>& w)
    {
      return w->is_active();
    });

    bool check_for_add = true;

    auto is_worker_needed = [this](size_t num_active_workers)
    {
      size_t tasks_per_thread = m_num_remaining_tasks / num_active_workers;
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
          worker_ptr->start();
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

  void work_group::erase_unnecesary_workers()
  {
    //TODO FILL

  }

  void work_group::background_thread_loop()
  {
    //while (!m_stop_background_thread)
    //{
    //  //Check if we need to move task group.
    //  {
    //    std::unique_lock lock(m_shared.m_mutex.m_task_group_changing_mutex);
    //    constexpr std::chrono::seconds time_out(2);
    //    //Wait with timeout
    //    bool success = m_shared.m_mutex.m_end_of_task_group_reached_condition.wait_for(lock, time_out, [this]()
    //    {
    //      const auto& r = m_shared.m_read;
    //      //Save to read the task_index even if it can be changed by the workers as we are already locking them with the lock above.
    //      bool const task_available_in_current_group = !r.m_task_groups.empty() && m_shared.m_write.m_task_idx < r.m_task_groups[r.m_task_group_idx].size();
    //      bool const there_are_more_groups_available = r.m_task_groups.size() > r.m_task_group_idx + 1;
    //      return m_stop_background_thread || (!task_available_in_current_group && there_are_more_groups_available);
    //    });
    //    if (m_stop_background_thread)
    //      break;
    //    if (success)
    //    {
    //      XMESSAGE("Advancing task group");
    //      advance_task_group_non_thread_safe();
    //      m_shared.m_mutex.m_task_added_condition.notify_one();
    //    }
    //  }
    //  //Check if we need to cleanup unused workers
    //  erase_unnecesary_workers();
    //}

  }

  void work_group::advance_task_group_non_thread_safe()
  {
    //We have finished with the current group
    //++m_shared.m_read.m_task_group_idx;
    //XMESSAGE("Incrementing group index to ", m_shared.m_read.m_task_group_idx.load());
    //m_shared.m_write.m_task_idx.store(0);
  }


}

#pragma endregion