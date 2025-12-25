/*
 * matmul_broken_A.c
 *
 * Compact style. Intentionally flawed: computes the inner-product 'sum' but
 * omits the write to the output matrix C. This is a silent bug: program runs
 * without crash but C remains unchanged (or zero if pre-zeroed).
 *
 * API:
 *   void matmul_broken_A(const double *A, const double *B, double *C, int N);
 *
 * Row-major layout: C[i*N + j]
 */

#include <stddef.h>

void matmul_broken_A(const double *A, const double *B, double *C, int N)
{
    int i, j, k;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            double sum = 0.0;
            for (k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            /* BUG (intentional): missing write to C.
             * Correct: C[i * N + j] = sum;
             */
        }
    }
}
