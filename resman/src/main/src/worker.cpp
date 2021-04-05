/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "main/work_group.h"

#pragma region worker

namespace WorkScheduling
{
#ifdef _DEBUG
#define XWORKERMSG(...) XMESSAGE("[Worker %d]", m_workerId); XMESSAGE(__VA_ARGS__);
  u32 Worker::s_workerIds = 0;
#else
#define XWORKERMSG(...)
#endif // _DEBUG

  Worker::Worker(WorkerData data, std::shared_mutex & mutex)
    : ASubroutine(data, mutex)
  {
#ifdef _DEBUG
    m_workerId = s_workerIds++;
#endif // _DEBUG
  }


  void Worker::VOnConditionMet()
  {
    //Get new task group
    //XASSERTMSG(m_data.m_task_groups.size() > m_data.m_task_group_idx, "Notifying worker when there are no available tasks");
    //As we are using a shared mutex it is possible that another worker has modifyed the index by this point
    auto groupIdx = m_data.taskGroupIdx++;
    if (m_data.taskGroups.size() > groupIdx)
    {
      m_taskGroup = m_data.taskGroups[groupIdx];
    }
    else
    {
      XWORKERMSG("Missed task after being notified");
      //We don't want to overflow if we missed the task
      --m_data.taskGroupIdx;
    }
  }
  
  void Worker::VPostConditionMet()
  {
    while (!m_taskGroup.empty())
    {
      Task nextTask = m_taskGroup.front();
      m_taskGroup.pop();
      nextTask();
      XWORKERMSG("TaskCompleted");
      m_data.parent.OnTaskCompleted();
    }
    XWORKERMSG("TaskGroupFinished");
    m_data.parent.OnTaskGroupEndReached();
  }

  bool Worker::VIsConditionMet() const
  {
    return m_data.taskGroups.size() > m_data.taskGroupIdx;
  }
}
#pragma endregion