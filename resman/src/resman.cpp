// resman.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "gtest/gtest.h"
#include <iostream>

GTEST_API_ int main(int argc, char** argv)
{

  // GTest
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
  //std::cout << "Hello World!\n"; 
}

TEST(geometry, in_plane_point)
{
  ASSERT_TRUE(1 == 1);

  EXPECT_EQ(1, 2);
  
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