/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"

/*
* Simple log for storing error messages
*/
class message_log
{
public://types
  enum message_flag
  {
    m_none    = 0,
    m_info    = 1 << 0,
    m_warning = 1 << 1,
    m_error   = 1 << 2
  };
  using message_flags = u32;
public:
  /*
  * Puts an error message into the log
  */
  void error(const str_t &);
  /*
  * Puts an warning message into the log
  */
  void warning(const str_t &);
  /*
  * Puts an info message into the log
  */
  void info(const str_t &);
  /*
  * Sets wich message types should be ignored by the log
  */
  void ignore_messages(message_flags = m_none);
  /*
  * Completely cleans the log and its allocated memory
  */
  void clear();
  /*
  * Cleans the log but keeps the used memory for later allocation
  */
  void clear_keep_mem();
  /*
  * Returns the current data stored on the log
  */
  std::vector<char> get_log()const;
  /*
  * Prints to the console the current data stored on the log
  */
  void print()const;
private:
  /*
  * Returns a pointer to a memory block of size 's' 
  */
  char * get_space(size_t s);
  /*
  * Appends a new message to the log
  */
  void push_message(const str_t &);
private:
  std::vector<char> m_data;                 //The full log
  size_t m_log_offset{0};                   //An offset in m_data representing where the log ends
  message_flags m_ignored_messages{m_none}; //The message types to ignore
};