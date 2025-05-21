#ifndef EC_CORE_H
#define EC_CORE_H
#include <stddef.h>
#include <assert.h>

typedef struct {
    size_t rows;
    size_t cols;
    float *data;
} ec_Matrixf32;

typedef struct {
    size_t rows;
    size_t cols;
    double *data;
} ec_Matrixf64;

static inline void ec_addf32(const ec_Matrixf32 *a, const ec_Matrixf32 *b, ec_Matrixf32 *out) {
    assert(a->rows == b->rows && a->cols == b->cols && a->rows == out->rows && a->cols == out->cols);
    for (size_t i = 0; i < a->rows * a->cols; ++i)
        out->data[i] = a->data[i] + b->data[i];
}

static inline void ec_addf64(const ec_Matrixf64 *a, const ec_Matrixf64 *b, ec_Matrixf64 *out) {
    assert(a->rows == b->rows && a->cols == b->cols && a->rows == out->rows && a->cols == out->cols);
    for (size_t i = 0; i < a->rows * a->cols; ++i)
        out->data[i] = a->data[i] + b->data[i];
}

#endif /* EC_CORE_H */
