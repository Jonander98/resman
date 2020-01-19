/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once
#include "pch.h"
#include "gtest/gtest.h"
#include "type_test.h"
#include "usage/texture.h"
#include "usage/model.h"

class dummy_resource : public resource { bool load(const file_path &)override { return false; } };

TEST(type_test, compile)
{
  reswoman<texture, model> rw;
  rw.get<dummy_resource>("hello");

}