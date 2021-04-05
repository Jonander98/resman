
#include "pch.h"
#include "main/subroutine.h"
#include "main/work_group.h"

namespace WorkScheduling
{
  ASubroutineBase::ASubroutineBase()
    : m_abort(false)
  {
  }

  bool ASubroutineBase::IsActive() const
  {
    return m_thread.joinable();
  }

  void ASubroutineBase::Start()
  {
    XASSERTMSG(!m_thread.joinable(), "[Subroutine] Calling start when already started");
    if (!m_thread.joinable())
    {
      m_abort = false;
      m_thread = std::thread(&ASubroutineBase::VThreadStartPoint, this);
    }
  }

  CleanupSubroutine::CleanupSubroutine(CleanupData data, std::shared_mutex& mutex)
    : ASubroutine(data, mutex)
  {
  }
  void CleanupSubroutine::VOnConditionMet()
  {
    XMESSAGE("%s Performing Cleanup", GetDebugName().c_str());
    auto& taskGroups = m_data.taskGroups;
    auto taskGroupIdx = m_data.taskGroupIdx.load();

    auto lastIt = std::next(taskGroups.begin(), taskGroupIdx);
    size_t numErasedElements = std::distance(taskGroups.begin(), lastIt);
    taskGroups.erase(taskGroups.begin(), lastIt);
    m_data.taskGroupIdx -= numErasedElements;
  }
  bool CleanupSubroutine::VIsConditionMet() const
  {
    //Condition will be that we have x ammount of empty/finished task groups
    constexpr size_t MAX_FINISHED_TASK_GROUPS = 50;

    return m_data.taskGroupIdx > MAX_FINISHED_TASK_GROUPS;
  }


  TaskMovingSubroutine::TaskMovingSubroutine(TaskMovingData data, std::shared_mutex& mutex, std::mutex& inputMutex)
    : ASubroutine(data, mutex)
    , m_lastMoveSize(0)
    , inputTaskMutex(inputMutex)
  {
  }
  void TaskMovingSubroutine::VOnConditionMet()
  {
    std::scoped_lock<std::mutex> lock(inputTaskMutex);
    m_lastMoveSize = m_data.m_input.size();
    std::move(m_data.m_input.begin(), m_data.m_input.end(), std::back_inserter(m_data.m_output));
    m_data.m_input.clear();
  }

  void TaskMovingSubroutine::VPostConditionMet()
  {
    m_data.m_parent.OnTaskMovedToSharedPool(m_lastMoveSize);
  }

  bool TaskMovingSubroutine::VIsConditionMet() const
  {
    constexpr size_t c_maxInputSize = 10;
    constexpr size_t c_minOutputSize = 5;

    const size_t numAvailableTasks = m_data.m_input.size();
    const bool manyInput = numAvailableTasks > c_maxInputSize;
    const bool lowOutputTask = (m_data.m_output.size() - m_data.taskGroupIdx) < c_minOutputSize;
    const bool shouldMove = manyInput || (lowOutputTask && numAvailableTasks > 0);
    return shouldMove;
  }
}