#include "Gauss_solve.h"
#include <cmath>
#include <stdexcept>


static constexpr double PIVOT_EPSILON = 1e-12; 
GaussVector Gauss_solve(GaussMatrix &ab)
{
    const Eigen::Index n = ab.rows();
    if (ab.cols() != n + 1)
        throw std::runtime_error(
            "Gauss_solve: augmented matrix must have shape n×(n+1), got " +
            std::to_string(n) + "×" + std::to_string(ab.cols()));

    for (Eigen::Index col = 0; col < n; ++col) {

        Eigen::Index pivot_row = col;
        double max_val = std::abs(ab(col, col));
        for (Eigen::Index row = col + 1; row < n; ++row) {
            double val = std::abs(ab(row, col));
            if (val > max_val) {
                max_val = val;
                pivot_row = row;
            }
        }

        if (max_val < PIVOT_EPSILON)
            throw std::runtime_error(
                "Gauss_solve: matrix is singular or nearly singular at step " +
                std::to_string(col));

        if (pivot_row != col)
            ab.row(col).swap(ab.row(pivot_row));

        const double pivot = ab(col, col);
        for (Eigen::Index row = col + 1; row < n; ++row) {
            double factor = ab(row, col) / pivot;
            ab.row(row) -= factor * ab.row(col);
            ab(row, col) = 0.0;
        }
    }

    GaussVector x(n);
    for (Eigen::Index i = n - 1; i >= 0; --i) {
        if (i + 1 < n)
            sum -= ab.row(i).segment(i + 1, n - 1 - i).dot(
                       x.segment(i + 1, n - 1 - i));
        x(i) = sum / ab(i, i);
    }

    return x;
}
