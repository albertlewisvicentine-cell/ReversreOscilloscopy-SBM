/*
 * test_harness.c
 *
 * Deterministic harness for exercising the three broken implementations.
 * Compile each harness target with its chosen implementation:
 *   gcc test_harness.c src/matmul_broken_A.c -O2 -lm -o test_run_A
 *
 * Usage:
 *   ./test_run_X [N] [scenario] [tolerance]
 *
 * Scenarios: increment, identity, random, pattern
 *
 * Exit codes:
 *   0 - no significant difference
 *   2 - mismatch detected (silent bug manifested)
 *   1 - internal error / invalid usage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/* matmul implementation provided by linking a chosen source file */
void matmul_broken_A(const double *A, const double *B, double *C, int N);
void matmul_broken_B(const double *A, const double *B, double *C, int N);
void matmul_broken_C(const double *A, const double *B, double *C, int N);

/* NOTE: one of the above will be linked into the final binary; the harness
 * calls the symbol `matmul_test_impl` which maps to whichever implementation
 * is provided during build via source selection. For simplicity we map it
 * here explicitly (weak linking not used). */
#ifdef BUILD_IMPL_A
#define matmul_test_impl matmul_broken_A
#elif defined(BUILD_IMPL_B)
#define matmul_test_impl matmul_broken_B
#elif defined(BUILD_IMPL_C)
#define matmul_test_impl matmul_broken_C
#else
/* If none selected, default to A to allow compilation sanity. */
#define matmul_test_impl matmul_broken_A
#endif

static void matmul_ref(const double *A, const double *B, double *C, int N)
{
    int i, j, k;
    for (i = 0; i < N; ++i) {
        for (j = 0; j < N; ++j) {
            double sum = 0.0;
            for (k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

static void fill_increment(double *A, double *B, int N)
{
    int i, j;
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j) {
            A[i * N + j] = (double)(i * N + j + 1);    /* 1..N*N */
            B[i * N + j] = (double)((i + j) % 7 - 3);  /* small signed values */
        }
}

static void fill_identity(double *A, double *B, int N)
{
    int i, j;
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j) {
            A[i * N + j] = (i == j) ? 1.0 : 0.0;
            B[i * N + j] = (i == j) ? 1.0 : 0.0;
        }
}

static void fill_random(double *A, double *B, int N)
{
    int i, j;
    unsigned seed = 123456789; /* deterministic seed */
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j) {
            seed = seed * 1103515245u + 12345u;
            A[i * N + j] = (double)((int)(seed >> 16) % 97 - 48) / 3.0;
            seed = seed * 1103515245u + 12345u;
            B[i * N + j] = (double)((int)(seed >> 16) % 61 - 30) / 4.0;
        }
}

static void fill_pattern(double *A, double *B, int N)
{
    int i, j;
    for (i = 0; i < N; ++i)
        for (j = 0; j < N; ++j) {
            A[i * N + j] = (double)(i + 1);
            B[i * N + j] = (double)(j + 2);
        }
}

static void usage(const char *prog)
{
    fprintf(stderr,
        "Usage: %s [N] [scenario] [tolerance]\n"
        "  N: matrix dimension (default 6)\n"
        "  scenario: increment | identity | random | pattern (default increment)\n"
        "  tolerance: max-diff tolerance (default 1e-12)\n", prog);
}

int main(int argc, char **argv)
{
    int N = 6;
    const char *scenario = "increment";
    double tolerance = 1e-12;

    if (argc >= 2) {
        N = atoi(argv[1]);
        if (N <= 0) {
            usage(argv[0]);
            return 1;
        }
    }
    if (argc >= 3) {
        scenario = argv[2];
    }
    if (argc >= 4) {
        tolerance = atof(argv[3]);
        if (tolerance < 0.0) tolerance = 1e-12;
    }

    size_t size = (size_t)N * N;
    double *A = malloc(size * sizeof(double));
    double *B = malloc(size * sizeof(double));
    double *C_test = malloc(size * sizeof(double));
    double *C_ref = malloc(size * sizeof(double));
    if (!A || !B || !C_test || !C_ref) {
        fprintf(stderr, "Allocation failed\n");
        free(A); free(B); free(C_test); free(C_ref);
        return 1;
    }

    /* fill according to scenario */
    if (strcmp(scenario, "increment") == 0) {
        fill_increment(A, B, N);
    } else if (strcmp(scenario, "identity") == 0) {
        fill_identity(A, B, N);
    } else if (strcmp(scenario, "random") == 0) {
        fill_random(A, B, N);
    } else if (strcmp(scenario, "pattern") == 0) {
        fill_pattern(A, B, N);
    } else {
        fprintf(stderr, "Unknown scenario '%s'\n", scenario);
        free(A); free(B); free(C_test); free(C_ref);
        return 1;
    }

    /* zero outputs (reasonable default) */
    for (size_t i = 0; i < size; ++i) {
        C_test[i] = 0.0;
        C_ref[i] = 0.0;
    }

    /* run broken implementation (linked in) */
    matmul_test_impl(A, B, C_test, N);

    /* compute reference */
    matmul_ref(A, B, C_ref, N);

    /* compare */
    double max_abs_diff = 0.0;
    double sum_abs_diff = 0.0;
    for (size_t i = 0; i < size; ++i) {
        double d = fabs(C_ref[i] - C_test[i]);
        sum_abs_diff += d;
        if (d > max_abs_diff) max_abs_diff = d;
    }

    printf("Matrix multiply test (impl: %s, N=%d, scenario=%s)\n",
           /* identify implementation by compiled symbol mapping */
#ifdef BUILD_IMPL_A
           "broken_A",
#elif defined(BUILD_IMPL_B)
           "broken_B",
#elif defined(BUILD_IMPL_C)
           "broken_C",
#else
           "broken_A (default)",
#endif
           N, scenario);
    printf("Max abs difference: %.6e\n", max_abs_diff);
    printf("Sum abs difference: %.6e\n", sum_abs_diff);

    /* print a small human-friendly table of reference vs broken outputs */
    printf("\nSample entries (i,j): ref | broken\n");
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int idx = i * N + j;
            printf("(%2d,%2d): %12.6g | %12.6g\n", i, j, C_ref[idx], C_test[idx]);
        }
        if (i >= 5 && N > 6) { /* keep output concise for large N */
            printf("... (truncated)\n");
            break;
        }
    }

    /* detection heuristic: test for non-zero max diff */
    if (max_abs_diff > tolerance) {
        fprintf(stderr, "\nDETECTED: numerical mismatch (max diff %.6e > tol %.6e)\n",
                max_abs_diff, tolerance);
        free(A); free(B); free(C_test); free(C_ref);
        return 2;
    }

    printf("\nNo significant difference detected (within tolerance %.6e).\n", tolerance);
    free(A); free(B); free(C_test); free(C_ref);
    return 0;
}
