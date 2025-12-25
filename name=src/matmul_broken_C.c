/*
 * matmul_broken_C.c
 *
 * Normal spacing, but accumulation performed in single-precision (float).
 * For large N or wide dynamic range values, this produces subtle numerical
 * differences due to reduced-precision accumulation (silent numerical error).
 *
 * API:
 *   void matmul_broken_C(const double *A, const double *B, double *C, int N);
 *
 * Row-major layout: C[i*N + j]
 */

#include <stddef.h>

void matmul_broken_C(const double *A, const double *B, double *C, int N)
{
    int i, j, k;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            /* BUG (intentional): accumulate in float (reduced precision) then cast */
            float sum_f = 0.0f;
            for (k = 0; k < N; ++k) {
                sum_f += (float)A[i * N + k] * (float)B[k * N + j];
            }
            C[i * N + j] = (double)sum_f;
        }
    }
}
