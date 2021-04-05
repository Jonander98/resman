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


TEST(ResourcePtr, creation)
{
  //Not compiling zone
  //resource_ptr<int> ptr;
  //resource_ptr<texture> tex_ptr(new texture);

  ASSERT_TRUE(ResourcePtr<Texture>().IsValid() == false);
  ASSERT_EQ(ResourcePtr<Texture>().GetReferenceCount(), i32(0));
}

TEST(ResourcePtr, reference_counting)
{
  Resman rm;
  rm.RegisterResource<Texture>();
  rm.Load<Texture>("./assets/test.png");
  ResourcePtr<Texture> t_ptr = rm.Get<Texture>("test.png");
  ASSERT_EQ(t_ptr.GetReferenceCount(), i32(1));
  ResourcePtr<Texture> t_ptr2 = t_ptr;
  ASSERT_EQ(t_ptr.GetReferenceCount(), i32(2));
  ASSERT_EQ(t_ptr.GetReferenceCount(), t_ptr2.GetReferenceCount());
}
