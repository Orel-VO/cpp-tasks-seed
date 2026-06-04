#include <iostream>
#include <string>

#include <Eigen/Dense>
#include <lazycsv.hpp>

#include "util.h"

int main(int argc, const char *argv[])
{
    auto A = load_csv_to_matrix(argv[1]);

    Eigen::MatrixXd B(3, 2); // ColMajor по-умолчанию
    B << 7, 8,
    9, 10,
    11, 12;

    Eigen::MatrixXd C = A * B;

    std::cout << "Матрица A:\n" << A << "\n\n";
    std::cout << "Матрица B:\n" << B << "\n\n";
    std::cout << "Результат умножения (C = A * B):\n" << C << "\n";

    // Редактирование на месте
    double c = 2.0;
    A.row(0) += c * A.row(1);
    A.coeffRef(1, 1) -= B.coeff(1, 1);
    std::cout << "Новая матрица A:\n" << A << "\n\n";

    return 0;
}
