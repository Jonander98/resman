
#include "pch.h"
#include "main/subroutine.h"
#include "main/work_group.h"

namespace work_scheduling
{
  subroutine_base::subroutine_base()
    : m_abort(false)
  {
  }

  bool subroutine_base::is_active() const
  {
    return m_thread.joinable();
  }

  void subroutine_base::start()
  {
    XASSERTMSG(!m_thread.joinable(), "[Subroutine] Calling start when already started");
    if (!m_thread.joinable())
    {
      m_abort = false;
      m_thread = std::thread(&subroutine_base::thread_start_point, this);
    }
  }

  cleanup_subroutine::cleanup_subroutine(cleanup_data data, std::shared_mutex& mutex)
    : subroutine(data, mutex)
  {
  }
  void cleanup_subroutine::on_condition_met()
  {
    XMESSAGE("Performing Cleanup");
    auto& task_groups = m_data.m_task_groups;
    auto task_group_idx = m_data.m_task_group_idx.load();

    //Clamp to make sure we are not erasing more than needed
    //task_group_idx = std::clamp(task_group_idx, task_group_idx, task_groups.size());

    size_t num_erased_elements = std::distance(task_groups.begin(), std::next(task_groups.begin(), task_group_idx));
    task_groups.erase(task_groups.begin(), std::next(task_groups.begin(), task_group_idx));
    m_data.m_task_group_idx -= num_erased_elements;
  }
  bool cleanup_subroutine::is_condition_met() const
  {
    //Condition will be that we have x ammount of empty/finished task groups
    constexpr size_t MAX_FINISHED_TASK_GROUPS = 50;

    return m_data.m_task_group_idx > MAX_FINISHED_TASK_GROUPS;
  }


  task_moving_subroutine::task_moving_subroutine(task_moving_data data, std::shared_mutex& mutex, std::mutex& input_mutex)
    : subroutine(data, mutex)
    , m_last_move_size(0)
    , m_input_task_mutex(input_mutex)
  {
  }
  void task_moving_subroutine::on_condition_met()
  {
    std::scoped_lock<std::mutex> lock(m_input_task_mutex);
    m_last_move_size = m_data.m_input.size();
    std::move(m_data.m_input.begin(), m_data.m_input.end(), std::back_inserter(m_data.m_output));
    m_data.m_input.clear();
  }

  void task_moving_subroutine::post_condition_met()
  {
    m_data.m_parent.on_task_moved_to_shared_pool(m_last_move_size);
  }

  bool task_moving_subroutine::is_condition_met() const
  {
    constexpr size_t MAX_INPUT_SIZE = 10;
    constexpr size_t MIN_OUTPUT_SIZE = 5;

    const size_t num_available_tasks = m_data.m_input.size();
    const bool many_input = num_available_tasks > MAX_INPUT_SIZE;
    const bool low_output_task = (m_data.m_output.size() - m_data.m_task_group_idx) < MIN_OUTPUT_SIZE;
    const bool should_move = many_input || (low_output_task && num_available_tasks > 0);
    return should_move;
  }
}