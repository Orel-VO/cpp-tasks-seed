#include <cctype>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <lazycsv.hpp>

#include "util.h"

namespace
{
bool parse_double(const std::string &s, double &value)
{
    try
    {
        size_t pos = 0;
        value = std::stod(s, &pos);
        while (pos < s.size())
        {
            if (!std::isspace(static_cast<unsigned char>(s[pos])))
            {
                return false;
            }
            ++pos;
        }
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}
}

GaussMatrix load_csv_to_matrix(const char *filename)
{
    std::vector<std::vector<double>> rcsv{};
    lazycsv::parser parser{ filename };
    size_t source_row = 0;
    size_t cols = 0;
    for (const auto row : parser)
    {
        ++source_row;
        std::vector<double> parsed_row{};
        bool parsed = true;
        for (const auto cell : row)
        {
            double value = 0.0;
            if (!parse_double(std::string(cell.raw()), value))
            {
                parsed = false;
                break;
            }
            parsed_row.push_back(value);
        }

        if (!parsed)
        {
            if (source_row == 1)
            {
                continue;
            }
            throw std::invalid_argument("invalid csv number");
        }
        if (parsed_row.empty())
        {
            continue;
        }
        if (cols == 0)
        {
            cols = parsed_row.size();
        }
        else if (parsed_row.size() != cols)
        {
            throw std::invalid_argument("non-rectangular csv");
        }
        rcsv.push_back(parsed_row);
    }

    if (rcsv.empty())
    {
        throw std::invalid_argument("empty csv");
    }

    GaussMatrix matrix(static_cast<int>(rcsv.size()), static_cast<int>(cols));
    for (int i = 0; i < matrix.rows(); ++i)
    {
        for (int j = 0; j < matrix.cols(); ++j)
        {
            matrix(i, j) = rcsv[static_cast<size_t>(i)][static_cast<size_t>(j)];
        }
    }
    return matrix;
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

void print_vector_as_csv(std::ostream& out, const GaussVector &vector, int prec)
{
    out << "X\n";
    out << std::fixed << std::setprecision(prec);
    for (int i = 0; i < vector.rows(); ++i)
    {
        out << vector(i) << '\n';
    }
}
