/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#include "pch.h"
#include "main/work_group.h"

#pragma region worker

namespace WorkScheduling
{
  WorkGroup::WorkGroup(Config cf)
    : m_config(cf)
  {
    {
      CleanupData data
      {
         m_shared.taskGroups,
         m_shared.taskGroupIdx
      };
      m_subroutines.asSingle.cleanup = std::make_unique<CleanupSubroutine>(data, m_shared.taskGroupChangingMutex);
    }
    {
      TaskMovingData data
      {
         m_shared.inputTasks,
         m_shared.taskGroups,
         m_shared.taskGroupIdx,
         *this
      };
      m_subroutines.asSingle.taskMoving = std::make_unique<TaskMovingSubroutine>(data, m_shared.taskGroupChangingMutex, m_shared.inputTaskMutex);
    }

    for (auto& sub : m_subroutines.asAll)
    {
      sub->Start();
    }
  }

  WorkGroup::~WorkGroup()
  {
    XMESSAGE("Destroying Workgroup");
  }

  void WorkGroup::AddTask(Task t)
  {
    XMESSAGE("Adding a task");
    AddTask(std::vector<Task>{t});
  }

  void WorkGroup::AddTask(std::vector<Task> tasks)
  {
    if (tasks.empty())
      return;

    {
      std::lock_guard<std::mutex> lock(m_shared.inputTaskMutex);
      m_shared.inputTasks.emplace_back(std::deque<Task>(tasks.begin(), tasks.end()));
    }

    //TODO MERGE or SEPARATE

    m_subroutines.asSingle.taskMoving->VNotifyIfConditionMet();
    m_numRemainingTasks += tasks.size();
    ActivateOrAddWorkerIfNeeded();
  }

  void WorkGroup::StopExecution()
  {
    for (auto& sub : m_subroutines.asAll)
    {
      sub->VStop();
    }
    for (auto& worker : m_workers)
    {
      worker->VStop();
    }
    m_workers.clear();
  }

  size_t WorkGroup::GetNumRemainingTasks() const
  {
    return m_numRemainingTasks;
  }

  void WorkGroup::OnTaskCompleted()
  {
    m_numRemainingTasks--;
  }

  void WorkGroup::OnTaskGroupEndReached()
  {
    //Update notification on selected subroutines
    m_subroutines.asSingle.cleanup->VNotifyIfConditionMet();
    //We might have not moved when adding a task because we already had many tasks in the shared pool,
    //So we notify here too
    m_subroutines.asSingle.taskMoving->VNotifyIfConditionMet();
  }

  void WorkGroup::OnTaskMovedToSharedPool(size_t numMovedTasks)
  {
    //Notify workers
    std::vector<decltype(m_workers)::iterator> activeWorkers;
    activeWorkers.reserve(numMovedTasks);
    for (auto it = m_workers.begin(); it != m_workers.end() && activeWorkers.size() != numMovedTasks; ++it)
    {
      if ((*it)->IsActive())
      {
        activeWorkers.push_back(it);
      }
    }
    for (auto& it : activeWorkers)
    {
      (*it)->VNotifyIfConditionMet();
    }
  }

  void WorkGroup::AddWorker()
  {
    XMESSAGE("New Worker Added");

    WorkerData data
    {
      m_shared.taskGroupIdx,
      m_shared.taskGroups,
      *this
    };
    m_workers.emplace_back(std::make_unique<Worker>(data, m_shared.taskGroupChangingMutex));
    m_workers.back()->Start();
  }

  void WorkGroup::ActivateOrAddWorkerIfNeeded()
  {
    if (m_workers.empty())
    {//Check if it is the first one
      XASSERT(GetNumRemainingTasks() != 0);
      AddWorker();
      return;
    }

    size_t numActiveWorkers = std::count_if(m_workers.begin(), m_workers.end(), [](const std::unique_ptr<Worker>& w)
    {
      return w->IsActive();
    });

    bool checkForAdd = true;

    auto fIsWorkerNeeded = [this](size_t numActiveWorkers)
    {
      size_t tasksPerThread = m_numRemainingTasks / numActiveWorkers;
      return tasksPerThread > m_config.minResourcesToFork && m_config.maxThreads > numActiveWorkers;
    };

    for (auto it = m_workers.begin(); it != m_workers.end(); ++it)
    {
      const auto& workerPtr = *it;
      if (!workerPtr->IsActive())
      {
        //Check if we should reactivate it
        if (fIsWorkerNeeded(numActiveWorkers))
        {
          //We should activate it as we need it
          workerPtr->Start();
          ++numActiveWorkers;
        }
        else
        {
          //Don't add if we already saw we don't need more workers
          checkForAdd = false;
          break;
        }
      }
    }
    //Add workers if needed
    if (checkForAdd)
    {
      while (fIsWorkerNeeded(numActiveWorkers))
      {
        AddWorker();
        ++numActiveWorkers;
      }
    }
  }
}

#pragma endregion