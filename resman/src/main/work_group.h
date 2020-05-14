/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"
#include "utils/filepath.h"

class resource;

class work_group
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
  using task = std::function<void(void)>;

private:
  class worker
  {
  public:
    worker(work_group&);
    //Join the thread
    ~worker();
    //Makes the worker start working again if it was closed
    void activate();
  private:
    //Function called by the thread to keep looking for tasks
    void thread_loop();
    //The thread used by the worker
    std::thread m_thread;
    //The group the worker belongs to
    work_group& m_work_group;
    //If the worker is closed
    std::atomic<bool> m_closed;
  };
public:
  //Wait for the thread to finish execution
  work_group(config);
  ~work_group();
  //Adds a task to the queue
  void add_task(task fn);
  //Adds a vector of tasks to the queue
  void add_task(std::vector<task> tasks);
private:
  //Ask for the next available task to be executed
  task request_next_task_ownership();
private:
  //The index of the task that needs to be gathered
  std::atomic<i32> m_task_idx;
  //All the tasks that have been requested
  std::vector<std::vector<task>> m_task_groups;
  //The indes of the task group we are processing
  u32 m_task_group_idx;
  //All the workers on the work group
  std::vector<worker> m_workers;
  //Config related with when we should create a new worker
  config m_config;
};