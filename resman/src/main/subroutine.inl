
//Too many arguments for assert macro
#pragma warning (push)
#pragma warning (disable:4002)
namespace WorkScheduling
{
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline ASubroutine<Data_T, Mutex_T, Lock_T>::~ASubroutine()
  {
    if (m_thread.joinable())
    {
      VStop();
    }
  }
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline ASubroutine<Data_T, Mutex_T, Lock_T>::ASubroutine(Data_T data, Mutex_T& mutex)
    : m_data(data)
    , m_mutex(mutex)
  {
  }
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline void ASubroutine<Data_T, Mutex_T, Lock_T>::VStop()
  {
    XASSERTMSG(m_thread.joinable(), "[Subroutine] Calling stop when not started");
    if (m_thread.joinable())
    {
      XMESSAGE("%s Closing...", GetDebugName().c_str());
      m_abort = true;
      m_checkCondition.notify_one();
      m_thread.join();
    }
  }
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline void ASubroutine<Data_T, Mutex_T, Lock_T>::VNotifyIfConditionMet()
  {
    if (VIsConditionMet())
    {
      m_checkCondition.notify_one();
    }
  }

  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline void ASubroutine<Data_T, Mutex_T, Lock_T>::VThreadStartPoint()
  {
    while (!m_abort)
    {
      {
        Lock_T lock(m_mutex);
        XMESSAGE("%s StartingToWait", GetDebugName().c_str());
        m_checkCondition.wait(lock, [this]() -> bool
        {
          return m_abort || VIsConditionMet();
        });
        XMESSAGE("%s FinishedWaiting", GetDebugName().c_str());
        if (m_abort)
        {
          //Don't bother performing any other task
          break;
        }
        VOnConditionMet();
      }
      VPostConditionMet();
    }
  }
}

#pragma warning (pop) //4002