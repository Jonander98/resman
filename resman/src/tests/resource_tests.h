/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once
#include "pch.h"
#include "gtest/gtest.h"
#include "main/resource.h"
#include "main/resman.h"
#include "usage/texture.h"


TEST(resource_ptr, creation)
{
  //Not compiling zone
  //resource_ptr<int> ptr;
  //resource_ptr<texture> tex_ptr(new texture);

  //ASSERT_TRUE(resource_ptr<texture>().is_valid() == false);
  //Get reference count returns the actual reference count - 1 as we assume on will be kept by the
  //resman and we don't want to tell the user that there is a reference left somewhere
  //ASSERT_EQ(resource_ptr<texture>().get_reference_count(), u32(-1));
}

TEST(resource_ptr, reference_counting)
{
  resman rm;
  rm.register_resource<texture>();
  rm.load<texture>("./assets/test.png");
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_EQ(t_ptr.get_reference_count(), i32(1));
  resource_ptr<texture> t_ptr2 = t_ptr;
  ASSERT_EQ(t_ptr.get_reference_count(), i32(2));
  ASSERT_EQ(t_ptr.get_reference_count(), t_ptr2.get_reference_count());
}
