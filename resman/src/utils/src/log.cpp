/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#include "pch.h"
#include "utils/log.h"

void log::error(const str_t & st)
{
  const char err[] = "Error: ";
  push_message(err + st);
}

void log::warning(const str_t & st)
{
  const char war[] = "Warning: ";
  push_message(war + st);
}

void log::info(const str_t & st)
{
  const char info[] = "Info: ";
  push_message(info + st);
}

void log::ignore_messages(message_flags flags)
{
  m_ignored_messages = flags;
}

void log::clear()
{
  m_data.clear();
  clear_keep_mem();
}

void log::clear_keep_mem()
{
  //Just ignore the already stored data
  m_log_offset = 0;
}

std::vector<char> log::get_log() const
{
  return std::vector<char>(m_data.begin(), std::next(m_data.begin(), m_log_offset));
}

void log::print_log() const
{
  std::cout << get_log().data() << std::endl;
}

char * log::get_space(size_t size)
{
  if (m_data.size() - m_log_offset < size)
    m_data.resize(m_data.size() + size);
  char * tmp = m_data.data() + m_log_offset;
  m_log_offset += size;
  return tmp;
}

void log::push_message(const str_t & st)
{
  char * ptr = get_space(st.size()+1);
  std::memcpy(ptr, st.c_str(), st.size());
  ptr[st.size() + 1] = '\n';
}
