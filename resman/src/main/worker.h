#pragma once

#include "pch.h"
#include "main/work_scheduling.h"
#include "main/subroutine.h"

namespace WorkScheduling
{

  struct worker_data
  {
    //The index of the task group we are processing
    std::atomic<size_t> & m_task_group_idx;
    //All the tasks that have been requested
    const std::vector<task_group> & m_task_groups;
    class i_work_group& m_parent;
  };


  class worker final : public subroutine<worker_data, std::shared_mutex, std::shared_lock<std::shared_mutex>>
  {
  public:
    worker(worker_data data, std::shared_mutex& mutex);
  private:
    void on_condition_met() override;
    void post_condition_met() override;
    bool is_condition_met() const override;
  private:
    task_group m_task_group;
#ifdef _DEBUG
    static u32 s_worker_ids;
    u32 m_worker_id;
    std::string get_debug_name() const override { return "[Worker]"; };
#endif // _DEBUG
  };

}