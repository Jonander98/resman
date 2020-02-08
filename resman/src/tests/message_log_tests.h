/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once
#include "pch.h"
#include "gtest/gtest.h"
#include "utils/message_log.h"


TEST(message_log, add_message)
{
  message_log log;
  log.error("Test Error");
  log.warning("Test Warning");
  log.info("Test Info");
  //log.print();
}
TEST(message_log, cleaning)
{
  message_log log;
  log.error("Test Error");
  log.warning("Test Warning");
  log.info("Test Info");
  log.clear();
  std::vector<char> v1 = log.get_data();
  log.error("Test Error");
  log.warning("Test Warning");
  log.info("Test Info");
  log.clear_keep_mem();
  std::vector<char> v2 = log.get_data();
  ASSERT_EQ(v1, v2);
}

TEST(message_log, cleaning2)
{
  message_log log;
  log.error("Test Error");
  std::vector<char> v1 = log.get_data();
  log.clear();
  log.error("Test Error");
  std::vector<char> v2 = log.get_data();
  log.clear_keep_mem();
  log.error("Test Error");
  std::vector<char> v3 = log.get_data();
  ASSERT_EQ(v1, v2);
  ASSERT_EQ(v1, v3);

}

TEST(message_log, ignore_message)
{
  message_log log;
  log.ignore_messages(message_log::message_flag::m_warning | message_log::message_flag::m_info);
  log.error("Test Error");
  log.warning("Test Warning");
  log.info("Test Info");
  message_log log2;
  log2.error("Test Error");

  ASSERT_EQ(log.get_data(), log2.get_data());
}
