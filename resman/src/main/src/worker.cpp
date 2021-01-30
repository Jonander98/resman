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

  worker::worker(worker_data data, std::shared_mutex & mutex)
    : subroutine(data, mutex)
  {
#ifdef _DEBUG
    m_worker_id = s_worker_ids++;
#endif // _DEBUG
  }


  void worker::on_condition_met()
  {
    //Get new task group
    //XASSERTMSG(m_data.m_task_groups.size() > m_data.m_task_group_idx, "Notifying worker when there are no available tasks");
    //As we are using a shared mutex it is possible that another worker has modifyed the index by this point
    auto group_idx = m_data.m_task_group_idx++;
    if (m_data.m_task_groups.size() > group_idx)
    {
      m_task_group = m_data.m_task_groups[group_idx];
    }
    else
    {
      XWORKERMSG("Missed task after being notified");
      //We don't want to overflow if we missed the task
      --m_data.m_task_group_idx;
    }
  }
  
  void worker::post_condition_met()
  {
    while (!m_task_group.empty())
    {
      task next_task = m_task_group.front();
      m_task_group.pop();
      next_task();
      XWORKERMSG("TaskCompleted");
      m_data.m_parent.on_task_completed();
    }
    XWORKERMSG("TaskGroupFinished");
    m_data.m_parent.on_task_group_end_reached();
  }

  bool worker::is_condition_met() const
  {
    return m_data.m_task_groups.size() > m_data.m_task_group_idx;
  }
}
#pragma endregion