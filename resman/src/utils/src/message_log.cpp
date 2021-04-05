/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "utils/message_log.h"

void MessageLog::Error(const str_t & st)
{
  if (m_ignoredMessages & EMessageFlag::f_error)
    return;
  const char err[] = "Error: ";
  PushMessage(err + st);
}

void MessageLog::Warning(const str_t & st)
{
  if (m_ignoredMessages & EMessageFlag::f_warning)
    return;
  const char war[] = "Warning: ";
  PushMessage(war + st);
}

void MessageLog::Info(const str_t & st)
{
  if (m_ignoredMessages & EMessageFlag::f_info)
    return;
  const char info[] = "Info: ";
  PushMessage(info + st);
}

void MessageLog::IgnoreMessages(MessageFlags flags)
{
  m_ignoredMessages = flags;
}

void MessageLog::Clear()
{
  m_data.clear();
  ClearKeepMem();
}

void MessageLog::ClearKeepMem()
{
  //Just ignore the already stored data
  m_logOffset = 0;
}

std::vector<char> MessageLog::GetData() const
{
  std::vector<char> tmp(m_data.begin(), std::next(m_data.begin(), m_logOffset));
  tmp.push_back(0);
  return tmp;
}

void MessageLog::Print() const
{
  std::cout << GetData().data() << std::endl;
}

bool MessageLog::IsEmpty() const
{
  return m_data.empty();
}

char * MessageLog::GetSpace(size_t size)
{
  if (m_data.size() - m_logOffset < size)
    m_data.resize(m_data.size() + size);
  char * tmp = m_data.data() + m_logOffset;
  m_logOffset += size;
  return tmp;
}

void MessageLog::PushMessage(const str_t & st)
{
  char * ptr = GetSpace(st.size()+1);
  std::memcpy(ptr, st.c_str(), st.size());
  ptr[st.size()] = '\n';
}
