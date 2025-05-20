#include "ec_core.h"
#include <stdio.h>

int main(void) {
    float a_data[4] = {1,2,3,4};
    float b_data[4] = {5,6,7,8};
    float c_data[4];
    ec_Matrixf32 A = {2,2,a_data};
    ec_Matrixf32 B = {2,2,b_data};
    ec_Matrixf32 C = {2,2,c_data};
    ec_addf32(&A, &B, &C);
    FILE *f = fopen("/tmp/c_out.txt", "w");
    for (size_t i=0; i<4; ++i) fprintf(f, "%f\n", c_data[i]);
    fclose(f);
    return 0;
}
