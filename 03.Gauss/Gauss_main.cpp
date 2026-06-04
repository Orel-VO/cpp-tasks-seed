#include <iostream>
#include <stdexcept>
#include <string>

#include "Gauss_solve.h"
#include "util.h"

int main(int argc, const char *argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0]
                  << " <augmented_matrix.csv> [solution.csv]\n";
        return 1;
    }

    try {
        // Загружаем расширенную матрицу
        GaussMatrix ab = load_csv_to_matrix(argv[1]);
        std::cerr << "Loaded " << ab.rows() << "×" << ab.cols()
                  << " augmented matrix from " << argv[1] << "\n";

        GaussMatrix ab_work = ab;
        GaussVector x = Gauss_solve(ab_work);

        if (argc >= 3) {
            save_vector_to_csv(x, argv[2]);
            std::cerr << "Solution written to " << argv[2] << "\n";
        } else {
            std::cout.precision(15);
            std::cout << std::scientific;
            for (Eigen::Index i = 0; i < x.size(); ++i)
                std::cout << x(i) << "\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
