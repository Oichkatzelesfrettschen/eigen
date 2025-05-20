#include <iostream>
#include <Eigen/Dense>

using namespace Eigen;

int main() {
    Matrix<float,2,2,RowMajor> A; A << 1,2,3,4;
    Matrix<float,2,2,RowMajor> B; B << 5,6,7,8;

    Matrix<float,2,2,RowMajor> C = A + B;
    Matrix<float,2,2,RowMajor> M = A * B;

    FullPivLU<Matrix<float,2,2,RowMajor>> lu(A);
    Matrix<float,2,2,RowMajor> L = Matrix<float,2,2,RowMajor>::Identity();
    L.triangularView<StrictlyLower>() = lu.matrixLU().triangularView<StrictlyLower>();
    Matrix<float,2,2,RowMajor> U = lu.matrixLU().triangularView<Upper>();

    HouseholderQR<Matrix<float,2,2,RowMajor>> qr(A);
    Matrix<float,2,2,RowMajor> Q = qr.householderQ();
    Matrix<float,2,2,RowMajor> R = qr.matrixQR().triangularView<Upper>();

    for(int i=0;i<4;i++) std::cout << C.data()[i] << " ";
    for(int i=0;i<4;i++) std::cout << M.data()[i] << " ";
    for(int i=0;i<4;i++) std::cout << L.data()[i] << " ";
    for(int i=0;i<4;i++) std::cout << U.data()[i] << " ";
    for(int i=0;i<4;i++) std::cout << Q.data()[i] << " ";
    for(int i=0;i<4;i++) std::cout << R.data()[i] << " ";
    return 0;
}
