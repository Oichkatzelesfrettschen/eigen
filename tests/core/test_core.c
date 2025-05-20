#include <stdio.h>
#include "../../eigenc/include/ec_core.h"

int main() {
    float ad[4] = {1,2,3,4};
    float bd[4] = {5,6,7,8};
    float cd[4];
    float md[4];
    float l[4];
    float u[4];
    float qd[4];
    float rd[4];

    ec_Matrixf32 A = {2,2,ad};
    ec_Matrixf32 B = {2,2,bd};
    ec_Matrixf32 C = {2,2,cd};
    ec_Matrixf32 M = {2,2,md};
    ec_Matrixf32 L = {2,2,l};
    ec_Matrixf32 U = {2,2,u};
    ec_Matrixf32 Q = {2,2,qd};
    ec_Matrixf32 R = {2,2,rd};

    ec_add(&A,&B,&C);
    ec_gemm(&A,&B,&M);
    ec_lu(&A,&L,&U);
    ec_qr(&A,&Q,&R);

    for(int i=0;i<4;i++) printf("%g ", cd[i]);
    for(int i=0;i<4;i++) printf("%g ", md[i]);
    for(int i=0;i<4;i++) printf("%g ", L.data[i]);
    for(int i=0;i<4;i++) printf("%g ", U.data[i]);
    for(int i=0;i<4;i++) printf("%g ", Q.data[i]);
    for(int i=0;i<4;i++) printf("%g ", R.data[i]);
    return 0;
}
