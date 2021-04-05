#pragma once

#include "pch.h"
#include "main/work_scheduling.h"
#include "main/subroutine.h"

namespace WorkScheduling
{

  struct WorkerData
  {
    //The index of the task group we are processing
    std::atomic<size_t> & taskGroupIdx;
    //All the tasks that have been requested
    const std::vector<TaskGroup> & taskGroups;
    class IWorkGroup& parent;
  };


  class Worker final : public ASubroutine<WorkerData, std::shared_mutex, std::shared_lock<std::shared_mutex>>
  {
  public:
    Worker(WorkerData data, std::shared_mutex& mutex);
  private:
    void VOnConditionMet() override;
    void VPostConditionMet() override;
    bool VIsConditionMet() const override;
  private:
    TaskGroup m_taskGroup;
#ifdef _DEBUG
    static u32 s_workerIds;
    u32 m_workerId;
    std::string GetDebugName() const override { return "[Worker]"; };
#endif // _DEBUG
  };

}