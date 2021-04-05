#pragma once
#include "pch.h"
#include "work_scheduling.h"

namespace WorkScheduling
{
  class ISubroutine
  {
  public:
    virtual ~ISubroutine() = default;
    ISubroutine() = default;
    virtual void Start() = 0;
    virtual void VStop() = 0;
    virtual void VNotifyIfConditionMet() = 0;

    ISubroutine(ISubroutine&& rhs) = default;
    ISubroutine& operator=(ISubroutine&& rhs) = default;
  private:
    ISubroutine(const ISubroutine& rhs) = delete;
    ISubroutine& operator=(const ISubroutine& rhs) = delete;
  };

  class ASubroutineBase : public ISubroutine
  {
  public:
    ASubroutineBase();
    virtual ~ASubroutineBase() = default;
    bool IsActive() const;
    void Start() override;
    virtual void VStop() = 0;
    ASubroutineBase(ASubroutineBase&& rhs) = default;
    ASubroutineBase& operator=(ASubroutineBase&& rhs) = default;
  protected:
    //With lock active
    virtual void VOnConditionMet() = 0;
    //With lock inactive
    virtual void VPostConditionMet() {};
    virtual bool VIsConditionMet() const = 0;
    virtual void VThreadStartPoint() = 0;
  protected:
    std::atomic<bool> m_abort;
    std::thread m_thread;
  };


  template <typename Data_T, typename Mutex_T, typename Lock_T = std::unique_lock<Mutex_T>>
  class ASubroutine : public ASubroutineBase
  {
    using condition_variable_t = typename std::conditional<std::is_base_of<std::shared_mutex, Mutex_T>::value, std::condition_variable_any, std::condition_variable>::type;
  public:
    virtual ~ASubroutine();
    ASubroutine(Data_T data, Mutex_T& mutex);
    void VStop() override;
    void VNotifyIfConditionMet() override;

    ASubroutine(ASubroutine&& rhs) = default;
    ASubroutine& operator=(ASubroutine&& rhs) = default;
  protected:
    Data_T m_data;
#ifdef _DEBUG
    virtual std::string GetDebugName() const { return "[Base Subroutine]"; };
#endif // _DEBUG

  private:
    void VThreadStartPoint() override;
    Mutex_T& m_mutex;
    condition_variable_t m_checkCondition;
  };

  struct CleanupData
  {
    std::vector<TaskGroup>& taskGroups;
    std::atomic<size_t>& taskGroupIdx;
  };
  class CleanupSubroutine final : public ASubroutine<CleanupData, std::shared_mutex>
  {
  public:
    CleanupSubroutine(CleanupData data, std::shared_mutex& mutex);
  protected:
    void VOnConditionMet() override;
    bool VIsConditionMet() const override;
#ifdef _DEBUG
    std::string GetDebugName() const override { return "[Cleanup]"; };
#endif // _DEBUG
  };

  struct TaskMovingData
  {
    std::vector<TaskGroup>& m_input;
    std::vector<TaskGroup>& m_output;
    const std::atomic<size_t>& taskGroupIdx;
    class IWorkGroup& m_parent;
  };
  class TaskMovingSubroutine final : public ASubroutine< TaskMovingData, std::shared_mutex>
  {
  public:
    TaskMovingSubroutine(TaskMovingData data, std::shared_mutex& mutex, std::mutex& input_mutex);
  private:
    void VOnConditionMet() override;
    void VPostConditionMet() override;
    bool VIsConditionMet() const override;
#ifdef _DEBUG
    std::string GetDebugName() const override { return "[TaskMove]"; };
#endif // _DEBUG
    size_t m_lastMoveSize;
    std::mutex& inputTaskMutex;
  };
}

#include "subroutine.inl"