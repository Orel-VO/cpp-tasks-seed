#include <random>
#include <stdexcept>

#include <gtest/gtest.h>

#include "Gauss_solve.h"
#include "util.h"

TEST(GaussSolve, SmallSolve)
{
    GaussMatrix ab(2, 3);
    ab << 2.0, 1.0, 5.0,
    1.0, -1.0, 1.0;

    const auto x = Gauss_solve(ab);
    EXPECT_NEAR(x(0), 2.0, 1e-10);
    EXPECT_NEAR(x(1), 1.0, 1e-10);
}

TEST(GaussSolve, UsesRowPivot)
{
    GaussMatrix ab(2, 3);
    ab << 0.0, 2.0, 4.0,
    3.0, 4.0, 11.0;

    const auto x = Gauss_solve(ab);
    EXPECT_NEAR(x(0), 1.0, 1e-10);
    EXPECT_NEAR(x(1), 2.0, 1e-10);
}

TEST(GaussSolve, SingularThrows)
{
    GaussMatrix ab(2, 3);
    ab << 1.0, 2.0, 3.0,
    2.0, 4.0, 6.0;

    EXPECT_THROW(Gauss_solve(ab), std::invalid_argument);
}

TEST(GaussSolve, ReproducibleLargeSystem)
{
    constexpr int n = 50;
    std::mt19937 gen(123456U);
    std::uniform_real_distribution<double> dist(-5.0, 5.0);

    GaussMatrix a(n, n);
    GaussVector expected(n);
    for (int i = 0; i < n; ++i)
    {
        expected(i) = dist(gen);
        for (int j = 0; j < n; ++j)
        {
            a(i, j) = dist(gen);
        }
        a(i, i) += static_cast<double>(n);
    }

    GaussMatrix ab(n, n + 1);
    ab.leftCols(n) = a;
    ab.col(n) = a * expected;

    const auto actual = Gauss_solve(ab);
    EXPECT_LE((actual - expected).cwiseAbs().maxCoeff(), 1e-8);
}

TEST(Csv, LoadMatrixWithHeader)
{
    const auto ab = load_csv_to_matrix("AB.csv");
    ASSERT_EQ(ab.rows(), 2);
    ASSERT_EQ(ab.cols(), 3);
    EXPECT_DOUBLE_EQ(ab(0, 0), 2.0);
    EXPECT_DOUBLE_EQ(ab(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(ab(0, 2), 5.0);
    EXPECT_DOUBLE_EQ(ab(1, 0), 1.0);
    EXPECT_DOUBLE_EQ(ab(1, 1), -1.0);
    EXPECT_DOUBLE_EQ(ab(1, 2), 1.0);
}
