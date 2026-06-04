#include <Eigen/Dense>

typedef Eigen::VectorXd /* Eigen::Matrix<double, Eigen::Dynamic, 1> */ GaussVector;
typedef Eigen::Matrix<double, -1, -1, Eigen::RowMajor> GaussMatrix;

GaussMatrix load_csv_to_matrix(const char *filename);
void print_matrix_as_csv(std::ostream& out, const GaussMatrix &matrix, int prec = 6);
