/*
 * matmul_broken_B.c
 *
 * Exaggerated spacing and layout for pedagogical clarity.
 * Intentional subtle bug: wrong indexing / effective transposition in the
 * source operand access, producing a valid-running program but wrong results.
 *
 * Example symptom: when B == Identity, result is NOT equal to A; instead a
 * transposed or scrambled variant appears.
 *
 * API:
 *   void matmul_broken_B(const double *A, const double *B, double *C, int N);
 *
 * Row-major layout: C[i*N + j]
 */

#include <stddef.h>

void matmul_broken_B(const double *A, const double *B, double *C, int N)
{
    int i, j, k;

    for ( i = 0 ; i < N ; ++i ) {

        for ( j = 0 ; j < N ; ++j ) {

            double sum = 0.0;

            for ( k = 0 ; k < N ; ++k ) {

                /* BUG (intentional):
                 * The access to A is swapped (k * N + i) instead of (i * N + k).
                 * This subtle change permutes the multiplicands and produces wrong
                 * but running output.
                 */
                sum += A[ k * N + i ] * B[ k * N + j ];
            }

            C[i * N + j] = sum;
        }
    }
}
