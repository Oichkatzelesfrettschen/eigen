#include "ec_core.h"
#include "ec_generated.h"
#include <stdio.h>

int main(void) {
    float a_data[4] = {1,2,3,4};
    float b_data[4] = {5,6,7,8};
    float add_data[4];
    float mul_data[4];

    ec_Matrixf32 A = {2,2,a_data};
    ec_Matrixf32 B = {2,2,b_data};
    ec_Matrixf32 C_add = {2,2,add_data};
    ec_addf32(&A, &B, &C_add);

    EC_Matrix2f Ag = {2,2,a_data};
    EC_Matrix2f Bg = {2,2,b_data};
    EC_Matrix2f C_mul = {2,2,mul_data};
    ec_mul(&Ag, &Bg, &C_mul);

    FILE *f = fopen("/tmp/c_out.txt", "w");
    for (size_t i=0; i<4; ++i) fprintf(f, "%f\n", add_data[i]);
    for (size_t i=0; i<4; ++i) fprintf(f, "%f\n", mul_data[i]);
    fclose(f);
    return 0;
}
