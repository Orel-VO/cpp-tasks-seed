#include "Gauss_solve.h"

#include <cmath>
#include <stdexcept>

GaussVector Gauss_solve(GaussMatrix &ab)
{
    const int rows = ab.rows();
    const int cols = ab.cols();
    if (rows <= 0 || cols != rows + 1)
    {
        throw std::invalid_argument("invalid augmented matrix size");
    }

    constexpr double eps = 1e-12;
    for (int col = 0; col < rows; ++col)
    {
        int pivot = col;
        double pivot_abs = std::abs(ab(col, col));
        for (int row = col + 1; row < rows; ++row)
        {
            const double current_abs = std::abs(ab(row, col));
            if (current_abs > pivot_abs)
            {
                pivot = row;
                pivot_abs = current_abs;
            }
        }

        if (pivot_abs < eps)
        {
            throw std::invalid_argument("singular system");
        }

        if (pivot != col)
        {
            ab.row(col).swap(ab.row(pivot));
        }

        ab.row(col) /= ab(col, col);
        for (int row = 0; row < rows; ++row)
        {
            if (row == col)
            {
                continue;
            }
            const double factor = ab(row, col);
            if (std::abs(factor) > eps)
            {
                ab.row(row) -= factor * ab.row(col);
            }
        }
    }

    return ab.col(cols - 1);
}
