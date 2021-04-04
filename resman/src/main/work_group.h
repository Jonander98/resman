/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "main/worker.h"
#include "main/work_scheduling.h"
#include "main/subroutine.h"

namespace work_scheduling
{
  class i_work_group
  {
  public:
    virtual void on_task_completed() = 0;
    virtual void on_task_group_end_reached() = 0;
    virtual void on_task_moved_to_shared_pool(size_t num_moved_tasks) = 0;
  };

  class work_group : public i_work_group
  {
  public:
    struct config
    {
      //Maximum number of threads allowed
      u8 max_threads{ 4 };
      /*
      * Minimum number of resources that have to be in the queue of a thread to create a new one
      * always respecting the maximum
      */
      u8 min_resources_to_fork{ 3 };
    };
  private:

    struct shared_work_group_data
    {
      //The index of the next task group to be processed
      std::atomic<size_t> m_task_group_idx{ 0 };
      //Shared pool of tasks with workers (Very demanded)
      std::vector<task_group> m_task_groups;
      //Mutex to block the workers from getting new tasks if any other subroutine is changing the task groups
      std::shared_mutex m_task_group_changing_mutex;
      //Tasks that will be later passed to the shared task group
      std::vector<task_group> m_input_tasks;
      //Mutex used to securely modify the input tasks
      std::mutex m_input_task_mutex;
    };
    union work_group_subroutines
    {
      work_group_subroutines() : m_all{} {}
      ~work_group_subroutines() {}
      struct as_struct
      {
        std::unique_ptr<cleanup_subroutine> m_cleanup;
        std::unique_ptr<task_moving_subroutine> m_task_moving;
      }m_single;
      static constexpr size_t num_subroutines = sizeof(as_struct) / sizeof(std::unique_ptr<i_subroutine>);
      std::array<std::unique_ptr<i_subroutine>, num_subroutines> m_all;
    };
  public:
    //Wait for the thread to finish execution
    work_group(config);
    virtual ~work_group();
    //Adds a task to the queue
    void add_task(task fn);
    //Adds a vector of tasks to the queue
    void add_task(std::vector<task> tasks);
    //Waits for the threads to finish the current tasks and removes the workers
    void stop_execution();
    //Returns the number of tasks that have not been completed yet
    size_t get_num_remaining_tasks() const;
  private:
    //Called by the worker when a task is completed
    void on_task_completed() override;
    //Called by the worker when it detects it has reached the end of the task group
    void on_task_group_end_reached() override;
    //Called by the task_move subroutine when a move has finished successfully
    void on_task_moved_to_shared_pool(size_t num_moved_tasks) override;
    //Helper function to add a new worker
    void add_worker();
    //Checks if an extra worker is needed due to the extra load and creates one if necessary
    void activate_or_add_worker_if_needed();
  private:
    //Shared data between workers
    shared_work_group_data m_shared;
    //All the workers on the work group.
    std::vector<std::unique_ptr<worker>> m_workers;
    //Config related with when we should create a new worker
    config m_config;
    //How many tasks are stored, but not complete
    size_t m_num_remaining_tasks;
    //Taks in the background
    work_group_subroutines m_subroutines;
  };
}
