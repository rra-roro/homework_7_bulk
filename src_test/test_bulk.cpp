#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "lib_version.h"
#include "matrix.h"

#define _TEST 1



TEST(version, test1)
{
      ASSERT_TRUE(version() > 0);
}

TEST(matrix, create_dimension1)
{
      roro_lib::matrix<int, -1, 1> matrix;
      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, create_dimension2)
{
      roro_lib::matrix<int, -1> matrix;
      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, create_dimension3)
{
      roro_lib::matrix<int, -1, 3> matrix;
      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, default_value_dimension1)
{
      roro_lib::matrix<int, -1, 1> matrix;
      ASSERT_TRUE(matrix[0] == -1);
      ASSERT_TRUE(matrix[34523] == -1);

      auto a = matrix[0];
      ASSERT_TRUE(a == -1);

      auto b = matrix[5234];
      ASSERT_TRUE(b == -1);

      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, default_value_dimension2)
{
      roro_lib::matrix<int, -1> matrix;
      ASSERT_TRUE(matrix[0][0] == -1);
      ASSERT_TRUE(matrix[34523][52435] == -1);

      auto a = matrix[0][0];
      ASSERT_TRUE(a == -1);

      auto b = matrix[434][5234];
      ASSERT_TRUE(b == -1);

      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, default_value_dimension3)
{
      roro_lib::matrix<int, -1, 3> matrix;
      ASSERT_TRUE(matrix[0][0][0] == -1);
      ASSERT_TRUE(matrix[34523][52435][67] == -1);

      auto a = matrix[0][0][0];
      ASSERT_TRUE(a == -1);

      auto b = matrix[434][5234][523];
      ASSERT_TRUE(b == -1);

      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, assignment_dimension1)
{
      roro_lib::matrix<int, -1, 1> matrix;
      matrix[100] = 314;
      ASSERT_TRUE(matrix[100] == 314);
      ASSERT_TRUE(matrix.size() == 1);

      matrix[12345] = 314;
      ASSERT_TRUE(matrix[12345] == 314);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, assignment_dimension2)
{
      roro_lib::matrix<int, -1> matrix;
      matrix[100][100] = 314;
      ASSERT_TRUE(matrix[100][100] == 314);
      ASSERT_TRUE(matrix.size() == 1);

      matrix[12345][54321] = 314;
      ASSERT_TRUE(matrix[12345][54321] == 314);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, assignment_dimension3)
{
      roro_lib::matrix<int, -1, 3> matrix;
      matrix[100][100][100] = 314;
      ASSERT_TRUE(matrix[100][100][100] == 314);
      ASSERT_TRUE(matrix.size() == 1);

      matrix[12345][54321][34524] = 314;
      ASSERT_TRUE(matrix[12345][54321][34524] == 314);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, ext_assignment_dimension1)
{
      roro_lib::matrix<int, -1, 1> matrix;
      (matrix[100] = 314) = 0;
      ASSERT_TRUE(matrix[100] == 0);
      ASSERT_TRUE(matrix.size() == 1);

      ((matrix[12345] = 314) = 0) = 100;
      ASSERT_TRUE(matrix[12345] == 100);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, ext_assignment_dimension2)
{
      roro_lib::matrix<int, -1> matrix;
      (matrix[100][100] = 314) = 0;
      ASSERT_TRUE(matrix[100][100] == 0);
      ASSERT_TRUE(matrix.size() == 1);

      ((matrix[12345][242] = 314) = 0) = 100;
      ASSERT_TRUE(matrix[12345][242] == 100);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, ext_assignment_dimension3)
{
      roro_lib::matrix<int, -1, 3> matrix;
      (matrix[100][100][100] = 314) = 0;
      ASSERT_TRUE(matrix[100][100][100] == 0);
      ASSERT_TRUE(matrix.size() == 1);

      ((matrix[12345][242][4] = 314) = 0) = 100;
      ASSERT_TRUE(matrix[12345][242][4] == 100);
      ASSERT_TRUE(matrix.size() == 2);
}

TEST(matrix, iterator_dimension1)
{
      roro_lib::matrix<int, -1, 1> matrix;

      matrix[100] = 314;

      for (auto c : matrix)
      {
            auto [column, v] = c;

            ASSERT_TRUE(column == 100);
            ASSERT_TRUE(v == 314);
      }
}

TEST(matrix, iterator_dimension2)
{
      roro_lib::matrix<int, -1> matrix;

      matrix[100][100] = 314;

      for (auto c : matrix)
      {
            auto [row, column, v] = c;

            ASSERT_TRUE(row == 100);
            ASSERT_TRUE(column == 100);
            ASSERT_TRUE(v == 314);
      }
}

TEST(matrix, iterator_dimension3)
{
      roro_lib::matrix<int, -1, 3> matrix;

      matrix[100][100][100] = 314;

      for (auto c : matrix)
      {
            auto [table, row, column, v] = c;

            ASSERT_TRUE(table == 100);
            ASSERT_TRUE(row == 100);
            ASSERT_TRUE(column == 100);
            ASSERT_TRUE(v == 314);
      }
}

TEST(matrix, assignment_default_dimension1)
{
      roro_lib::matrix<int, -1, 1> matrix;
      matrix[100] = 314;
      matrix[12345] = 314;
      ASSERT_TRUE(matrix[100] == 314 && matrix[12345] == 314);
      ASSERT_TRUE(matrix.size() == 2);

      matrix[100] = -1;
      matrix[12345] = -1;
      ASSERT_TRUE(matrix[100] == -1 && matrix[12345] == -1);
      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, assignment_default_dimension2)
{
      roro_lib::matrix<int, -1> matrix;
      matrix[100][100] = 314;
      matrix[12345][12345] = 314;
      ASSERT_TRUE(matrix[100][100] == 314 && matrix[12345][12345] == 314);
      ASSERT_TRUE(matrix.size() == 2);

      matrix[100][100] = -1;
      matrix[12345][12345] = -1;
      ASSERT_TRUE(matrix[100][100] == -1 && matrix[12345][12345] == -1);
      ASSERT_TRUE(matrix.size() == 0);
}

TEST(matrix, assignment_default_dimension3)
{
      roro_lib::matrix<int, -1, 3> matrix;
      matrix[100][100][100] = 314;
      matrix[12345][12345][12345] = 314;
      ASSERT_TRUE(matrix[100][100][100] == 314 && matrix[12345][12345][12345] == 314);
      ASSERT_TRUE(matrix.size() == 2);

      matrix[100][100][100] = -1;
      matrix[12345][12345][12345] = -1;
      ASSERT_TRUE(matrix[100][100][100] == -1 && matrix[12345][12345][12345] == -1);
      ASSERT_TRUE(matrix.size() == 0);
}
