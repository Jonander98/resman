/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "utils/message_log.h"

void message_log::error(const str_t & st)
{
  if (m_ignored_messages & message_flag::m_error)
    return;
  const char err[] = "Error: ";
  push_message(err + st);
}

void message_log::warning(const str_t & st)
{
  if (m_ignored_messages & message_flag::m_warning)
    return;
  const char war[] = "Warning: ";
  push_message(war + st);
}

void message_log::info(const str_t & st)
{
  if (m_ignored_messages & message_flag::m_info)
    return;
  const char info[] = "Info: ";
  push_message(info + st);
}

void message_log::ignore_messages(message_flags flags)
{
  m_ignored_messages = flags;
}

void message_log::clear()
{
  m_data.clear();
  clear_keep_mem();
}

void message_log::clear_keep_mem()
{
  //Just ignore the already stored data
  m_log_offset = 0;
}

std::vector<char> message_log::get_log() const
{
  std::vector<char> tmp(m_data.begin(), std::next(m_data.begin(), m_log_offset));
  tmp.push_back(0);
  return tmp;
}

void message_log::print() const
{
  std::cout << get_log().data() << std::endl;
}

char * message_log::get_space(size_t size)
{
  if (m_data.size() - m_log_offset < size)
    m_data.resize(m_data.size() + size);
  char * tmp = m_data.data() + m_log_offset;
  m_log_offset += size;
  return tmp;
}

void message_log::push_message(const str_t & st)
{
  char * ptr = get_space(st.size()+1);
  std::memcpy(ptr, st.c_str(), st.size());
  ptr[st.size()] = '\n';
}
