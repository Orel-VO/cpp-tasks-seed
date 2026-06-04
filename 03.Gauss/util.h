#ifndef GAUSS_UTIL_H
#define GAUSS_UTIL_H

#include <Eigen/Dense>

#include <iosfwd>

typedef Eigen::VectorXd /* Eigen::Matrix<double, Eigen::Dynamic, 1> */ GaussVector;
typedef Eigen::Matrix<double, -1, -1, Eigen::RowMajor> GaussMatrix;

GaussMatrix load_csv_to_matrix(const char *filename);
void print_matrix_as_csv(std::ostream& out, const GaussMatrix &matrix, int prec = 6);
void print_vector_as_csv(std::ostream& out, const GaussVector &vector, int prec = 6);

#endif
