/**
 * @author  Jon Ander Jimenez
 * @contact  jonander.jimenez@gmail.com
 */

#pragma once
#include "pch.h"
#include "gtest/gtest.h"
#include "main/resman.h"
#include "usage/texture.h"
#include "usage/model.h"


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

TEST(resman, resource_request_no_load)
{
  resman rm;
  rm.register_resource<texture>();
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_TRUE(!t_ptr.is_valid());

  //texture t;
  //ASSERT_TRUE(typeid(t) == typeid(*reinterpret_cast<resource*>(&t)));

}
TEST(resman, resource_request_load_syncronous)
{
  resman rm;
  rm.register_resource<texture>();
  rm.load<texture>("./assets/test.png");
  resource_ptr<texture> t_ptr = rm.get<texture>("test.png");
  ASSERT_TRUE(t_ptr.is_valid());
}

TEST(resman, resource_request_load_syncronous2)
{
  resman rm;
  rm.register_resource<texture, model>();
  rm.load<texture>("./assets/test.png");
  rm.load<model>("./assets/test.model");
  rm.load<texture>("./assets/test.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(1));
  ASSERT_EQ(rm.get_all_t<model>().size(), size_t(1));
  rm.load<texture>("./assets/test2.png");
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(2));
  //rm.get_log().print();
}
TEST(resman, resource_unload)
{
  resman rm;
  rm.register_resource<texture>();
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
TEST(resman, resource_unload_all_t)
{
  resman rm;
  rm.register_resource<texture>();
  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.unload_all_t<texture>();
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));

  //rm.get_log().print();
}
TEST(resman, resource_unload_all)
{
  resman rm;
  rm.register_resource<texture, model>();
  rm.load<texture>("./assets/test.png");
  rm.load<texture>("./assets/test2.png");
  rm.load<texture>("./assets/test3.png");
  rm.load<model>("./assets/test.obj");
  rm.unload_all();
  ASSERT_EQ(rm.get_all_t<texture>().size(), size_t(0));
  ASSERT_EQ(rm.get_all_t<model>().size(), size_t(0));

  //rm.get_log().print();
}
//Examples
#if 0

namespace {


  TEST(geometry, in_project_point_plane)
  {
    std::ifstream file("../tests/geometry/in_project_point_plane", std::ios::in);
    ASSERT_TRUE(file.is_open());

    int line = 0;
    while (!file.eof()) {
      line++;
      const auto plane = read_plane(file);
      const auto point = read_point(file, true);
      const auto projection = read_point(file, true);
      const auto result = project_point_plane(point, plane);
      EXPECT_NEAR(result[0], projection[0], 0.01f) << "[Line " << line << "]";
      EXPECT_NEAR(result[1], projection[1], 0.01f) << "[Line " << line << "]";
      EXPECT_NEAR(result[2], projection[2], 0.01f) << "[Line " << line << "]";
    }
  }


  TEST(geometry, in_ray_plane)
  {
    std::ifstream file("../tests/geometry/in_ray_plane", std::ios::in);
    ASSERT_TRUE(file.is_open());

    int line = 0;
    while (!file.eof()) {
      line++;
      std::string placeholder;
      const auto  ray = read_ray(file);
      const auto  plane = read_plane(file);
      float       expected = 0.0f;
      file >> expected;
      const float intersection_time = intersection_ray_plane(ray, plane);
      EXPECT_FLOAT_EQ(intersection_time, expected) << "[Line " << line << "]";
    }
  }

  TEST(geometry, in_ray_triangle)
  {
    std::ifstream file("../tests/geometry/in_ray_triangle", std::ios::in);
    ASSERT_TRUE(file.is_open());

    int line = 0;
    while (!file.eof()) {
      line++;
      std::string placeholder;
      const auto  ray = read_ray(file);
      const auto  triangle = read_triangle(file);
      float       expected = 0.0f;
      file >> expected;
      const float intersection_time = intersection_ray_triangle(ray, triangle);
      EXPECT_FLOAT_EQ_DECS(intersection_time, expected) << "[Line " << line << "]";
    }
  }

  TEST(geometry, in_ray_sphere)
  {
    std::ifstream file("../tests/geometry/in_ray_sphere", std::ios::in);
    ASSERT_TRUE(file.is_open());

    int line = 0;
    while (!file.eof()) {
      line++;
      std::string placeholder;
      const auto  ray = read_ray(file);
      const auto  sphere = read_sphere(file);
      float       expected = 0.0f;
      file >> expected;
      const float intersection_time = intersection_ray_sphere(ray, sphere);
      EXPECT_FLOAT_EQ_DECS(intersection_time, expected) << "[Line " << line << "]";
    }
  }
}

namespace {
  TEST(bounding_volumes, in_aabb_centroid)
  {
    std::ifstream file("../tests/bounding_volumes/in_aabb_centroid", std::ios::in);
    ASSERT_TRUE(file.is_open());

    int line = 0;
    while (!file.eof()) {
      line++;
      const auto points = read_points(file);
      const auto expected = read_aabb(file);
      const auto result = aabb_from_points(points);
      ASSERT_TRUE(are_points_inside(points, result)) << "[Line " << line << "]";
      ASSERT_NEAR(result.min_point.x, expected.min_point.x, 0.01f) << "[Line " << line << "]";
      ASSERT_NEAR(result.min_point.y, expected.min_point.y, 0.01f) << "[Line " << line << "]";
      ASSERT_NEAR(result.min_point.z, expected.min_point.z, 0.01f) << "[Line " << line << "]";
      ASSERT_NEAR(result.max_point.x, expected.max_point.x, 0.01f) << "[Line " << line << "]";
      ASSERT_NEAR(result.max_point.y, expected.max_point.y, 0.01f) << "[Line " << line << "]";
      ASSERT_NEAR(result.max_point.z, expected.max_point.z, 0.01f) << "[Line " << line << "]";
    }
  }
}

#endif