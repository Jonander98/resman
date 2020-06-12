/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "resman_tests.h"


TEST(resman, resource_registration)
{
  resman rm;
  rm.register_resource<texture>();
  ASSERT_TRUE(rm.is_registered<texture>());

  resman rm2;
  rm2.register_resource<texture, model>();
  ASSERT_TRUE(rm2.is_registered<texture>());
  ASSERT_TRUE(rm2.is_registered<model>());
}

TEST_F(resman_fixture, resource_request_no_load)
{
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_TRUE(!t_ptr.is_valid());

  //texture t;
  //ASSERT_TRUE(typeid(t) == typeid(*reinterpret_cast<resource*>(&t)));

}
TEST_F(resman_fixture, resource_request_load_syncronous)
{
  rm.load<texture>("./assets/test.png");
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_TRUE(t_ptr.is_valid());
}

TEST_F(resman_fixture, resource_request_load_syncronous2)
{

  rm.load<texture>("./assets/test.png");
  rm.load<model>("./assets/test.model");
  rm.load<texture>("./assets/test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(1));
  ASSERT_EQ(rm.get_all_t<model>().size(), size_t(1));
  rm.load<texture>("./assets/test2.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(2));
  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload)
{

  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test.png");
  rm.unload<texture>("test.png");
  rm.unload<texture>("test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(2));
  rm.unload<texture>("test2.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(1));
  ASSERT_TRUE(rm.get<texture>("test.png").is_valid());

  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload_all_t)
{

  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.unload_all_t<texture>();
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));

  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload_all)
{
  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.load<model>("./assets/test.obj");
  rm.unload_all();
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));
  ASSERT_EQ(rm.get_all_t<model>().size(), size_t(0));

  //rm.get_log().print();
}

TEST_F(resman_fixture, resource_status)
{
  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.load<model>("./assets/test.obj");

  rm.get<texture>("test.png");
  auto use = rm.get<texture>("test2.png");

  auto log = rm.get_resource_manager_status();

  log.print();

  //rm.get_log().print();
}

TEST_F(resman_fixture, resource_save_to_file_then_load)
{
  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.load<model>("./assets/test.obj");

  rm.save_to_file("./saved_resources.reslist");
  rm.from_file<texture, model>("./saved_resources.reslist");

  ASSERT_TRUE(rm.get<texture>("test.png").is_valid());

}
