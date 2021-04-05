/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */
#pragma once

#include "resman_tests.h"


TEST(Resman, resource_registration)
{
  Resman rm;
  rm.RegisterResource<Texture>();
  ASSERT_TRUE(rm.IsRegistered<Texture>());

  Resman rm2;
  rm2.RegisterResource<Texture, Model>();
  ASSERT_TRUE(rm2.IsRegistered<Texture>());
  ASSERT_TRUE(rm2.IsRegistered<Model>());
}

TEST_F(resman_fixture, resource_request_no_load)
{
  ResourcePtr<Texture> t_ptr = rm.Get<Texture>("test.png");
  ASSERT_TRUE(!t_ptr.IsValid());

  //texture t;
  //ASSERT_TRUE(typeid(t) == typeid(*reinterpret_cast<resource*>(&t)));

}
TEST_F(resman_fixture, resource_request_load_syncronous)
{
  rm.Load<Texture>("./assets/test.png");
  ResourcePtr<Texture> t_ptr = rm.Get<Texture>("test.png");
  ASSERT_TRUE(t_ptr.IsValid());
}

TEST_F(resman_fixture, resource_request_load_syncronous2)
{

  rm.Load<Texture>("./assets/test.png");
  rm.Load<Model>("./assets/test.model");
  rm.Load<Texture>("./assets/test.png");
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(1));
  ASSERT_EQ(rm.GetAllOfType<Model>().size(), size_t(1));
  rm.Load<Texture>("./assets/test2.png");
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(2));
  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload)
{

  rm.Load<Texture>("./assets/test.png");
  rm.Load<Texture>("./assets/test.png");
  rm.Unload<Texture>("test.png");
  rm.Unload<Texture>("test.png");
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(0));
  rm.Load<Texture>("./assets/test2.png");
  rm.Load<Texture>("./assets/test.png");
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(2));
  rm.Unload<Texture>("test2.png");
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(1));
  ASSERT_TRUE(rm.Get<Texture>("test.png").IsValid());

  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload_all_t)
{

  rm.Load<Texture>("./assets/test.png");
  rm.Load<Texture>("./assets/test2.png");
  rm.Load<Texture>("./assets/test3.png");
  rm.UnloadAllOfType<Texture>();
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(0));

  //rm.get_log().print();
}
TEST_F(resman_fixture, resource_unload_all)
{
  rm.Load<Texture>("./assets/test.png");
  rm.Load<Texture>("./assets/test2.png");
  rm.Load<Texture>("./assets/test3.png");
  rm.Load<Model>("./assets/test.obj");
  rm.UnloadAll();
  ASSERT_EQ(rm.GetAllOfType<Texture>().size(), size_t(0));
  ASSERT_EQ(rm.GetAllOfType<Model>().size(), size_t(0));

  //rm.get_log().print();
}

TEST_F(resman_fixture, resource_status)
{
  rm.Load<Texture>("./assets/test.png");
  rm.Load<Texture>("./assets/test2.png");
  rm.Load<Texture>("./assets/test3.png");
  rm.Load<Model>("./assets/test.obj");

  rm.Get<Texture>("test.png");
  auto use = rm.Get<Texture>("test2.png");

  auto log = rm.GetResourceManagerStatus();

  log.Print();

  //rm.get_log().print();
}

TEST_F(resman_fixture, resource_save_to_file_then_load)
{
  rm.Load<Texture>("./assets/test.png");
  rm.Load<Texture>("./assets/test2.png");
  rm.Load<Texture>("./assets/test3.png");
  rm.Load<Model>("./assets/test.obj");

  rm.SaveToFile("./saved_resources.reslist");
  rm.FromFile<Texture, Model>("./saved_resources.reslist");

  ASSERT_TRUE(rm.Get<Texture>("test.png").IsValid());

}
