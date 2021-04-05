/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/worker.h"
#include "main/work_scheduling.h"
#include "main/subroutine.h"

namespace WorkScheduling
{
  class IWorkGroup
  {
  public:
    virtual void OnTaskCompleted() = 0;
    virtual void OnTaskGroupEndReached() = 0;
    virtual void OnTaskMovedToSharedPool(size_t numMovedTasks) = 0;
  };

  class WorkGroup : public IWorkGroup
  {
  public:
    struct Config
    {
      //Maximum number of threads allowed
      u8 maxThreads{ 4 };
      /*
      * Minimum number of resources that have to be in the queue of a thread to create a new one
      * always respecting the maximum
      */
      u8 minResourcesToFork{ 3 };
    };
  private:

    struct SharedWorkGroupData
    {
      //The index of the next task group to be processed
      std::atomic<size_t> taskGroupIdx{ 0 };
      //Shared pool of tasks with workers (Very demanded)
      std::vector<TaskGroup> taskGroups;
      //Mutex to block the workers from getting new tasks if any other subroutine is changing the task groups
      std::shared_mutex taskGroupChangingMutex;
      //Tasks that will be later passed to the shared task group
      std::vector<TaskGroup> inputTasks;
      //Mutex used to securely modify the input tasks
      std::mutex inputTaskMutex;
    };
    union UWorkGroupSubroutines
    {
      UWorkGroupSubroutines() : asAll{} {}
      ~UWorkGroupSubroutines() {}
      struct AsStruct
      {
        std::unique_ptr<CleanupSubroutine> cleanup;
        std::unique_ptr<TaskMovingSubroutine> taskMoving;
      }asSingle;
      static constexpr size_t c_numSubroutines = sizeof(AsStruct) / sizeof(std::unique_ptr<ISubroutine>);
      std::array<std::unique_ptr<ISubroutine>, c_numSubroutines> asAll;
    };
  public:
    //Wait for the thread to finish execution
    WorkGroup(Config);
    virtual ~WorkGroup();
    //Adds a task to the queue
    void AddTask(Task fn);
    //Adds a vector of tasks to the queue
    void AddTask(std::vector<Task> tasks);
    //Waits for the threads to finish the current tasks and removes the workers
    void StopExecution();
    //Returns the number of tasks that have not been completed yet
    size_t GetNumRemainingTasks() const;
  private:
    //Called by the worker when a task is completed
    void OnTaskCompleted() override;
    //Called by the worker when it detects it has reached the end of the task group
    void OnTaskGroupEndReached() override;
    //Called by the task_move subroutine when a move has finished successfully
    void OnTaskMovedToSharedPool(size_t numMovedTasks) override;
    //Helper function to add a new worker
    void AddWorker();
    //Checks if an extra worker is needed due to the extra load and creates one if necessary
    void ActivateOrAddWorkerIfNeeded();
  private:
    //Shared data between workers
    SharedWorkGroupData m_shared;
    //All the workers on the work group.
    std::vector<std::unique_ptr<Worker>> m_workers;
    //Config related with when we should create a new worker
    Config m_config;
    //How many tasks are stored, but not complete
    size_t m_numRemainingTasks;
    //Taks in the background
    UWorkGroupSubroutines m_subroutines;
  };
}
