#pragma once
#include "pch.h"
#include "work_scheduling.h"

namespace WorkScheduling
{
  class i_subroutine
  {
  public:
    virtual ~i_subroutine() = default;
    i_subroutine() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void notify_if_condition_met() = 0;

    i_subroutine(i_subroutine&& rhs) = default;
    i_subroutine& operator=(i_subroutine&& rhs) = default;
  private:
    i_subroutine(const i_subroutine& rhs) = delete;
    i_subroutine& operator=(const i_subroutine& rhs) = delete;
  };

  class subroutine_base : public i_subroutine
  {
  public:
    subroutine_base();
    virtual ~subroutine_base() = default;
    bool is_active() const;
    void start() override;
    virtual void stop() = 0;
    subroutine_base(subroutine_base&& rhs) = default;
    subroutine_base& operator=(subroutine_base&& rhs) = default;
  protected:
    //With lock active
    virtual void on_condition_met() = 0;
    //With lock inactive
    virtual void post_condition_met() {};
    virtual bool is_condition_met() const = 0;
    virtual void thread_start_point() = 0;
  protected:
    std::atomic<bool> m_abort;
    std::thread m_thread;
  };


  template <typename Data_T, typename Mutex_T, typename Lock_T = std::unique_lock<Mutex_T>>
  class subroutine : public subroutine_base
  {
    using condition_variable_t = typename std::conditional<std::is_base_of<std::shared_mutex, Mutex_T>::value, std::condition_variable_any, std::condition_variable>::type;
  public:
    virtual ~subroutine();
    subroutine(Data_T data, Mutex_T& mutex);
    void stop() override;
    void notify_if_condition_met() override;

    subroutine(subroutine&& rhs) = default;
    subroutine& operator=(subroutine&& rhs) = default;
  protected:
    Data_T m_data;
#ifdef _DEBUG
    virtual std::string get_debug_name() const { return "[Base Subroutine]"; };
#endif // _DEBUG

  private:
    void thread_start_point() override;
    Mutex_T& m_mutex;
    condition_variable_t m_check_condition;
  };

  struct cleanup_data
  {
    std::vector<task_group>& m_task_groups;
    std::atomic<size_t>& m_task_group_idx;
  };
  class cleanup_subroutine final : public subroutine<cleanup_data, std::shared_mutex>
  {
  public:
    cleanup_subroutine(cleanup_data data, std::shared_mutex& mutex);
  protected:
    void on_condition_met() override;
    bool is_condition_met() const override;
#ifdef _DEBUG
    std::string get_debug_name() const override { return "[Cleanup]"; };
#endif // _DEBUG
  };

  struct task_moving_data
  {
    std::vector<task_group>& m_input;
    std::vector<task_group>& m_output;
    const std::atomic<size_t>& m_task_group_idx;
    class i_work_group& m_parent;
  };
  class task_moving_subroutine final : public subroutine< task_moving_data, std::shared_mutex>
  {
  public:
    task_moving_subroutine(task_moving_data data, std::shared_mutex& mutex, std::mutex& input_mutex);
  private:
    void on_condition_met() override;
    void post_condition_met() override;
    bool is_condition_met() const override;
#ifdef _DEBUG
    std::string get_debug_name() const override { return "[TaskMove]"; };
#endif // _DEBUG
    size_t m_last_move_size;
    std::mutex& m_input_task_mutex;
  };
}

#include "subroutine.inl"