/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 * Your code in trans.c must compile without warnings to receive credit.
 * requirement:
• You are allowed to define at most 12 local variables of type int per transpose function.1
• You are not allowed to side-step the previous rule by using any variables of type long or by using
any bit tricks to store more than one value to a single variable.
• Your transpose function may not use recursion.
• If you choose to use helper functions, you may not have more than 12 local variables on the stack
at a time between your helper functions and your top level transpose function. For example, if your
transpose declares 8 variables, and then you call a function which uses 4 variables, which calls another
function which uses 2, you will have 14 variables on the stack, and you will be in violation of the rule.
• Your transpose function may not modify array A. You may, however, do whatever you want with the
contents of array B.
• You are NOT allowed to define any arrays in your code or to use any variant of malloc.
 */
char transpose_submit_desc[] = "Transpose submission";
void bijk(int** A, int** B, int** C, int n,  int bsize)
{
    int i, j , k, kk, jj;
    double sum;
    int en = bsize * (n / bsize);
    for (i = 0; i < n; i++)
        for(j = 0; j < n; j++)
            C[i][j] = 0;
    for (kk = 0; kk < en; kk += bsize) {
        for (jj = 0; jj < en; jj += bsize) {
            for (i = 0; i < n; i++) {
                for (j = jj; j < jj + bsize; j++) {
                    sum = C[i][j];
                    for (k = kk; k < kk + bsize; k++) {
                    sum += A[i][k]*B[k][j];
                    }
                    C[i][j] = sum;
                }
            }
        }
    }
}
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, kk, jj, tmp;
    for (i = 0; i < M; i++)
    {
        for (j = 0; j < N; N++)
        {
            B[i][j] = 0;
        }
    }
    int bsize = 8;
    for (kk = 0; kk < N; kk += bsize)
    {
        for (jj = 0; jj < M; jj += bsize)
        {
            for (i = kk; i < (kk + bsize); i++)
            {
                for (j = jj; j < (jj + bsize); j++)
                {
                    tmp = A[i][j];
                    B[j][i] = tmp;
                }
            }
        }
    }
    for (i = kk; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
    for (i = 0; i < kk; i++)
    {
        for (j = kk; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc);
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

