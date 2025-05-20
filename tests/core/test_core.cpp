#include <Eigen/Dense>
#include <fstream>

int main() {
    Eigen::Matrix2f A; A << 1,2,3,4;
    Eigen::Matrix2f B; B << 5,6,7,8;
    Eigen::Matrix2f C = A + B;
    std::ofstream f("/tmp/cpp_out.txt");
    for(int i=0;i<2;i++)
        for(int j=0;j<2;j++)
            f << C(i,j) << "\n";
    return 0;
}
