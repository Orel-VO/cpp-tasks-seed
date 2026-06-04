#include <string>
#include <iomanip>
#include <lazycsv.hpp>

#include "util.h"

GaussMatrix load_csv_to_matrix(const char *filename)
{
    std::vector<std::vector<double>> rcsv{};
    {
        lazycsv::parser parser{ filename };
        for (const auto row : parser)
        {
            std::vector<double> r{};
            for (const auto cell : row)
            {
                r.push_back(std::stod(std::string(cell.raw())));
            }
            rcsv.push_back(r);
        }
    }

    return GaussMatrix(rcsv.size(), rcsv.begin()->size());
}

void print_matrix_as_csv(std::ostream& out, const GaussMatrix &matrix, int prec)
{
    for (int j = 0; j < matrix.cols(); ++j)
        out << "A,";
    out << "B\n";

    out << std::fixed << std::setprecision(prec);

    for (int i = 0; i < matrix.rows(); ++i)
    {
        for (int j = 0; j < matrix.cols(); ++j)
        {
            out << matrix(i, j);
            if (j < matrix.cols() - 1)
            {
                out << ',';
            }
        }
        out << '\n';
    }
}
