#include <fstream>

int main() {
    float A[4] = {1,2,3,4};
    float B[4] = {5,6,7,8};
    float C_add[4];
    float C_mul[4];

    for(int i=0;i<4;i++)
        C_add[i] = A[i] + B[i];

    for(int i=0;i<2;i++)
        for(int j=0;j<2;j++) {
            float sum = 0;
            for(int k=0;k<2;k++)
                sum += A[i*2+k]*B[k*2+j];
            C_mul[i*2+j] = sum;
        }

    std::ofstream f("/tmp/cpp_out.txt");
    for(int i=0;i<4;i++)
        f << C_add[i] << "\n";
    for(int i=0;i<4;i++)
        f << C_mul[i] << "\n";
    return 0;
}
