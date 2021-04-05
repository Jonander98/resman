/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "pch.h"

/*
* Simple log for storing error messages
*/
class MessageLog
{
public://types
  using MessageFlags = u32;
  enum EMessageFlag : MessageFlags
  {
    f_none    = 0,
    f_info    = 1 << 0,
    f_warning = 1 << 1,
    f_error   = 1 << 2
  };
public:
  /*
  * Puts an error message into the log
  */
  void Error(const str_t &);
  /*
  * Puts an warning message into the log
  */
  void Warning(const str_t &);
  /*
  * Puts an info message into the log
  */
  void Info(const str_t &);
  /*
  * Sets wich message types should be ignored by the log
  */
  void IgnoreMessages(MessageFlags = f_none);
  /*
  * Completely cleans the log and its allocated memory
  */
  void Clear();
  /*
  * Cleans the log but keeps the used memory for later allocation
  */
  void ClearKeepMem();
  /*
  * Returns the current data stored on the log
  */
  std::vector<char> GetData()const;
  /*
  * Prints to the console the current data stored on the log
  */
  void Print()const;
  /*
  * Returns true if the log is empty
  */
  bool IsEmpty()const;
private:
  /*
  * Returns a pointer to a memory block of size 's' 
  */
  char * GetSpace(size_t s);
  /*
  * Appends a new message to the log
  */
  void PushMessage(const str_t &);
private:
  std::vector<char> m_data;               //The full log
  size_t m_logOffset{0};                  //An offset in m_data representing where the log ends
  MessageFlags m_ignoredMessages{f_none}; //The message types to ignore
};