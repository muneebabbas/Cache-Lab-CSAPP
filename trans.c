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
void trans(int M, int N, int A[N][M], int B[M][N]);


/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int row, col, i, j;
    if (N == 32 && M == 32)
    {
        int temp, diagonal;
        for (row = 0; row < N; row += 8)
            for (col = 0; col < N; col += 8)
                for (i = row; i < row + 8; i++){
                    for (j = col; j < col + 8; j++){
                        if (i == j)
                        {
                            temp = A[i][j];
                            diagonal = j;
                        }
                        else
                        {
                            B[j][i] = A[i][j];
                        }
                    }

                    if (row == col)
                        B[diagonal][diagonal] = temp;
                }
                    

    }

    else if(N == 64 && M == 64)
    {
        int var1, var2, var3, var4;
        int var5, var6, var7, var8;
        
        for (row = 0; row < N; row += 8)
            for (col = 0; col < N; col += 8)
                    {
                        // We have an 8 by 8 matrix, logically divide it into 4 four by four matrices
                        for (i = 0; i < 4; ++i)
                        {
                            var1 = A[row + i][col];
                            var2 = A[row + i][col+1];
                            var3 = A[row + i][col+2];
                            var4 = A[row + i][col+3];

                            var5 = A[row + i][col+4];
                            var6 = A[row + i][col+5];
                            var7 = A[row + i][col+6];
                            var8 = A[row + i][col+7];

                            B[col][row+4+i] = var5;
                            B[col+1][row + 4+i] = var6;
                            B[col+2][row + 4+i] = var7;
                            B[col+3][row + 4+i] = var8;   

                            B[col][row + i] = var1;
                            B[col+1][row + i] = var2;
                            B[col+2][row + i] = var3;
                            B[col+3][row + i] = var4;

                        }                            

                        for(i = 0; i < 4; ++i)
                        {
                            var1 = B[col+i][row + 4];
                            var2 = B[col+i][row + 5];
                            var3 = B[col+i][row + 6];
                            var4 = B[col+i][row + 7];

                            for (j = row + 4; j < row + 8; ++j)
                            {
                                B[col+i][j] = A[j][col+i];
                            }

                            B[col+4+i][row + 0] = var1;
                            B[col+4+i][row + 1] = var2;
                            B[col+4+i][row + 2] = var3;
                            B[col+4+i][row + 3] = var4;

                        }
                        
                        for (i = 0; i < 4; ++i)
                        {
                            var1 = A[row+i+4][col+4];
                            var2 = A[row+i+4][col+5];
                            var3 = A[row+i+4][col+6];
                            var4 = A[row+i+4][col+7];

                            B[col+4][row+4+i] = var1;
                            B[col+5][row+4+i] = var2;
                            B[col+6][row+4+i] = var3;
                            B[col+7][row+4+i] = var4;
                        }

                    }

    }

    else
    {
        trans(M, N, A, B);
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
    registerTransFunction(trans, trans_desc); 

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

