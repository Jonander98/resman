
//Too many arguments for assert macro
#pragma warning (push)
#pragma warning (disable:4002)
namespace work_scheduling
{
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline subroutine<Data_T, Mutex_T, Lock_T>::~subroutine()
  {
    if (m_thread.joinable())
    {
      stop();
    }
  }
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline subroutine<Data_T, Mutex_T, Lock_T>::subroutine(Data_T data, Mutex_T& mutex)
    : m_data(data)
    , m_mutex(mutex)
  {
  }
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline void subroutine<Data_T, Mutex_T, Lock_T>::stop()
  {
    XASSERTMSG(m_thread.joinable(), "[Subroutine] Calling stop when not started");
    if (m_thread.joinable())
    {
      XMESSAGE("%s Closing...", get_debug_name().c_str());
      m_abort = true;
      m_check_condition.notify_one();
      m_thread.join();
    }
  }
  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline void subroutine<Data_T, Mutex_T, Lock_T>::notify_if_condition_met()
  {
    if (is_condition_met())
    {
      m_check_condition.notify_one();
    }
  }

  template<typename Data_T, typename Mutex_T, typename Lock_T>
  inline void subroutine<Data_T, Mutex_T, Lock_T>::thread_start_point()
  {
    while (!m_abort)
    {
      {
        Lock_T lock(m_mutex);
        XMESSAGE("%s StartingToWait", get_debug_name().c_str());
        m_check_condition.wait(lock, [this]() -> bool
        {
          return m_abort || is_condition_met();
        });
        XMESSAGE("%s FinishedWaiting", get_debug_name().c_str());
        if (m_abort)
        {
          //Don't bother performing any other task
          break;
        }
        on_condition_met();
      }
      post_condition_met();
    }
  }
}

#pragma warning (pop) //4002