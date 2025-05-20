#ifndef EC_CORE_H
#define EC_CORE_H
#include <stddef.h>
#include <math.h>

#ifdef EC_HAVE_BLAS
#include <cblas.h>
#endif

typedef struct { size_t rows, cols; float *data; } ec_Matrixf32;
typedef struct { size_t rows, cols; double *data; } ec_Matrixf64;

static inline void ec_addf32(const ec_Matrixf32 *A, const ec_Matrixf32 *B, ec_Matrixf32 *C) {
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<A->cols;j++)
            C->data[i*C->cols+j] = A->data[i*A->cols+j] + B->data[i*B->cols+j];
}
static inline void ec_addf64(const ec_Matrixf64 *A, const ec_Matrixf64 *B, ec_Matrixf64 *C) {
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<A->cols;j++)
            C->data[i*C->cols+j] = A->data[i*A->cols+j] + B->data[i*B->cols+j];
}

static inline void ec_subf32(const ec_Matrixf32 *A, const ec_Matrixf32 *B, ec_Matrixf32 *C) {
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<A->cols;j++)
            C->data[i*C->cols+j] = A->data[i*A->cols+j] - B->data[i*B->cols+j];
}
static inline void ec_subf64(const ec_Matrixf64 *A, const ec_Matrixf64 *B, ec_Matrixf64 *C) {
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<A->cols;j++)
            C->data[i*C->cols+j] = A->data[i*A->cols+j] - B->data[i*B->cols+j];
}

static inline void ec_transposef32(const ec_Matrixf32 *A, ec_Matrixf32 *T) {
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<A->cols;j++)
            T->data[j*T->cols+i] = A->data[i*A->cols+j];
}
static inline void ec_transposef64(const ec_Matrixf64 *A, ec_Matrixf64 *T) {
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<A->cols;j++)
            T->data[j*T->cols+i] = A->data[i*A->cols+j];
}

static inline void ec_gemm_f32(const ec_Matrixf32 *A, const ec_Matrixf32 *B, ec_Matrixf32 *C) {
#ifdef EC_HAVE_BLAS
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                (int)A->rows, (int)B->cols, (int)A->cols,
                1.0f, A->data, (int)A->cols,
                B->data, (int)B->cols,
                0.0f, C->data, (int)C->cols);
#else
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<B->cols;j++) {
            float sum = 0.0f;
            for (size_t k=0;k<A->cols;k++)
                sum += A->data[i*A->cols+k] * B->data[k*B->cols+j];
            C->data[i*C->cols+j] = sum;
        }
#endif
}
static inline void ec_gemm_f64(const ec_Matrixf64 *A, const ec_Matrixf64 *B, ec_Matrixf64 *C) {
#ifdef EC_HAVE_BLAS
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                (int)A->rows, (int)B->cols, (int)A->cols,
                1.0, A->data, (int)A->cols,
                B->data, (int)B->cols,
                0.0, C->data, (int)C->cols);
#else
    for (size_t i=0;i<A->rows;i++)
        for (size_t j=0;j<B->cols;j++) {
            double sum = 0.0;
            for (size_t k=0;k<A->cols;k++)
                sum += A->data[i*A->cols+k] * B->data[k*B->cols+j];
            C->data[i*C->cols+j] = sum;
        }
#endif
}

static inline void ec_lu_f32(const ec_Matrixf32 *A, ec_Matrixf32 *L, ec_Matrixf32 *U) {
    size_t n = A->rows;
    for (size_t i=0;i<n;i++) {
        for (size_t k=i;k<n;k++) {
            float sum = 0.0f;
            for (size_t j=0;j<i;j++) sum += L->data[i*n+j]*U->data[j*n+k];
            U->data[i*n+k] = A->data[i*A->cols+k] - sum;
        }
        for (size_t k=i;k<n;k++) {
            if (i==k) L->data[i*n+i] = 1.0f;
            else {
                float sum = 0.0f;
                for (size_t j=0;j<i;j++) sum += L->data[k*n+j]*U->data[j*n+i];
                L->data[k*n+i] = (A->data[k*A->cols+i] - sum) / U->data[i*n+i];
            }
        }
    }
}
static inline void ec_lu_f64(const ec_Matrixf64 *A, ec_Matrixf64 *L, ec_Matrixf64 *U) {
    size_t n = A->rows;
    for (size_t i=0;i<n;i++) {
        for (size_t k=i;k<n;k++) {
            double sum = 0.0;
            for (size_t j=0;j<i;j++) sum += L->data[i*n+j]*U->data[j*n+k];
            U->data[i*n+k] = A->data[i*A->cols+k] - sum;
        }
        for (size_t k=i;k<n;k++) {
            if (i==k) L->data[i*n+i] = 1.0;
            else {
                double sum = 0.0;
                for (size_t j=0;j<i;j++) sum += L->data[k*n+j]*U->data[j*n+i];
                L->data[k*n+i] = (A->data[k*A->cols+i] - sum) / U->data[i*n+i];
            }
        }
    }
}

static inline void ec_qr_f32(const ec_Matrixf32 *A, ec_Matrixf32 *Q, ec_Matrixf32 *R) {
    size_t n = A->cols;
    float *q = Q->data;
    float *r = R->data;
    for (size_t k=0;k<n;k++) {
        for (size_t i=0;i<A->rows;i++) {
            q[i*n+k] = A->data[i*A->cols+k];
            for (size_t j=0;j<k;j++)
                q[i*n+k] -= r[j*n+k]*q[i*n+j];
        }
        float norm=0.0f;
        for (size_t i=0;i<A->rows;i++) norm += q[i*n+k]*q[i*n+k];
        norm = sqrtf(norm);
        r[k*n+k] = norm;
        for (size_t i=0;i<A->rows;i++) q[i*n+k] /= norm;
        for (size_t j=k+1;j<n;j++) {
            r[k*n+j]=0.0f;
            for (size_t i=0;i<A->rows;i++) r[k*n+j] += q[i*n+k]*A->data[i*A->cols+j];
        }
    }
}
static inline void ec_qr_f64(const ec_Matrixf64 *A, ec_Matrixf64 *Q, ec_Matrixf64 *R) {
    size_t n = A->cols;
    double *q = Q->data;
    double *r = R->data;
    for (size_t k=0;k<n;k++) {
        for (size_t i=0;i<A->rows;i++) {
            q[i*n+k] = A->data[i*A->cols+k];
            for (size_t j=0;j<k;j++)
                q[i*n+k] -= r[j*n+k]*q[i*n+j];
        }
        double norm=0.0;
        for (size_t i=0;i<A->rows;i++) norm += q[i*n+k]*q[i*n+k];
        norm = sqrt(norm);
        r[k*n+k] = norm;
        for (size_t i=0;i<A->rows;i++) q[i*n+k] /= norm;
        for (size_t j=k+1;j<n;j++) {
            r[k*n+j]=0.0;
            for (size_t i=0;i<A->rows;i++) r[k*n+j] += q[i*n+k]*A->data[i*A->cols+j];
        }
    }
}

#define ec_add(A,B,C) _Generic(*(A), \
    ec_Matrixf32: ec_addf32, \
    ec_Matrixf64: ec_addf64)(A,B,C)

#define ec_sub(A,B,C) _Generic(*(A), \
    ec_Matrixf32: ec_subf32, \
    ec_Matrixf64: ec_subf64)(A,B,C)

#define ec_transpose(A,T) _Generic(*(A), \
    ec_Matrixf32: ec_transposef32, \
    ec_Matrixf64: ec_transposef64)(A,T)

#define ec_gemm(A,B,C) _Generic(*(A), \
    ec_Matrixf32: ec_gemm_f32, \
    ec_Matrixf64: ec_gemm_f64)(A,B,C)

#define ec_lu(A,L,U) _Generic(*(A), \
    ec_Matrixf32: ec_lu_f32, \
    ec_Matrixf64: ec_lu_f64)(A,L,U)

#define ec_qr(A,Q,R) _Generic(*(A), \
    ec_Matrixf32: ec_qr_f32, \
    ec_Matrixf64: ec_qr_f64)(A,Q,R)

#endif // EC_CORE_H
