#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <cmath>
#include <random>
#include <stdexcept>

#include "Gauss_solve.h"
#include "util.h"

static double residual_norm(const GaussMatrix &A, const GaussVector &b,
                             const GaussVector &x)
{
    double denom = b.norm();
    if (denom == 0.0) return (A * x).norm();
    return (A * x - b).norm() / denom;
}

TEST_CASE("1x1 system")
{
    GaussMatrix ab(1, 2);
    ab << 3.0, 9.0;   
    GaussVector x = Gauss_solve(ab);
    CHECK(x.size() == 1);
    CHECK(x(0) == doctest::Approx(3.0).epsilon(1e-12));
}

TEST_CASE("3x3 known system")
{
    GaussMatrix ab(3, 4);
    ab <<  2,  1, -1,  8,
          -3, -1,  2, -11,
          -2,  1,  2, -3;

    GaussVector x = Gauss_solve(ab);
    REQUIRE(x.size() == 3);
    CHECK(x(0) == doctest::Approx(2.0).epsilon(1e-10));
    CHECK(x(1) == doctest::Approx(3.0).epsilon(1e-10));
    CHECK(x(2) == doctest::Approx(-1.0).epsilon(1e-10));
}

TEST_CASE("diagonal system")
{
    const int n = 5;
    GaussMatrix ab = GaussMatrix::Zero(n, n + 1);
    for (int i = 0; i < n; ++i) {
        ab(i, i) = static_cast<double>(i + 1); 
        ab(i, n) = static_cast<double>(i + 1);  
    }
    GaussVector x = Gauss_solve(ab);
    for (int i = 0; i < n; ++i)
        CHECK(x(i) == doctest::Approx(1.0).epsilon(1e-12));
}

TEST_CASE("pivot required")
{
    GaussMatrix ab(3, 4);
    ab << 0, 2, 1, 5,
          2, 1, 1, 6,
          1, 1, 0, 3;

    GaussMatrix A = ab.leftCols(3);
    GaussVector b = ab.col(3);
    GaussVector x = Gauss_solve(ab);

    CHECK(residual_norm(A, b, x) < 1e-12);
}

TEST_CASE("singular matrix throws")
{
    GaussMatrix ab(3, 4);
    ab << 1, 2, 3, 6,
          2, 4, 6, 12,  
          1, 1, 1,  3;
    CHECK_THROWS_AS(Gauss_solve(ab), std::runtime_error);
}

TEST_CASE("wrong shape throws")
{
    GaussMatrix ab(3, 3);  
    ab.setZero();
    CHECK_THROWS_AS(Gauss_solve(ab), std::runtime_error);
}

TEST_CASE("large random diagonally-dominant system n=200")
{
    const int n = 200;
    const unsigned seed = 42;
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> off(-1.0, 1.0);
    std::uniform_real_distribution<double> rhs(-10.0, 10.0);

    GaussMatrix ab(n, n + 1);
    for (int i = 0; i < n; ++i) {
        double row_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            double v = off(rng);
            ab(i, j) = v;
            row_sum += std::abs(v);
        }
        ab(i, i) = row_sum + 1.0 + std::abs(off(rng)) * 0.5;
        ab(i, n) = rhs(rng);
    }

    GaussMatrix A = ab.leftCols(n);
    GaussVector b = ab.col(n);

    GaussVector x = Gauss_solve(ab);
    double res = residual_norm(A, b, x);
    INFO("residual = " << res);
    CHECK(res < 1e-10);
}

TEST_CASE("exact solution x_i = i+1")
{
    const int n = 6;
    std::mt19937 rng(123);
    std::uniform_real_distribution<double> off(-0.5, 0.5);

    GaussMatrix A(n, n);
    for (int i = 0; i < n; ++i) {
        double row_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i == j) continue;
            double v = off(rng);
            A(i, j) = v;
            row_sum += std::abs(v);
        }
        A(i, i) = row_sum + 1.0;
    }

    GaussVector x_exact(n);
    for (int i = 0; i < n; ++i) x_exact(i) = i + 1;
    GaussVector b = A * x_exact;

    GaussMatrix ab(n, n + 1);
    ab.leftCols(n) = A;
    ab.col(n) = b;

    GaussVector x = Gauss_solve(ab);
    for (int i = 0; i < n; ++i)
        CHECK(x(i) == doctest::Approx(x_exact(i)).epsilon(1e-10));
}
