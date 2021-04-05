/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once
#include "pch.h"
#include "gtest/gtest.h"
#include "utils/message_log.h"


TEST(MessageLog, add_message)
{
  MessageLog log;
  log.Error("Test Error");
  log.Warning("Test Warning");
  log.Info("Test Info");
  //log.print();
}
TEST(MessageLog, cleaning)
{
  MessageLog log;
  log.Error("Test Error");
  log.Warning("Test Warning");
  log.Info("Test Info");
  log.Clear();
  std::vector<char> v1 = log.GetData();
  log.Error("Test Error");
  log.Warning("Test Warning");
  log.Info("Test Info");
  log.ClearKeepMem();
  std::vector<char> v2 = log.GetData();
  ASSERT_EQ(v1, v2);
}

TEST(MessageLog, cleaning2)
{
  MessageLog log;
  log.Error("Test Error");
  std::vector<char> v1 = log.GetData();
  log.Clear();
  log.Error("Test Error");
  std::vector<char> v2 = log.GetData();
  log.ClearKeepMem();
  log.Error("Test Error");
  std::vector<char> v3 = log.GetData();
  ASSERT_EQ(v1, v2);
  ASSERT_EQ(v1, v3);

}

TEST(MessageLog, ignore_message)
{
  MessageLog log;
  log.IgnoreMessages(MessageLog::EMessageFlag::f_warning | MessageLog::EMessageFlag::f_info);
  log.Error("Test Error");
  log.Warning("Test Warning");
  log.Info("Test Info");
  MessageLog log2;
  log2.Error("Test Error");

  ASSERT_EQ(log.GetData(), log2.GetData());
}
