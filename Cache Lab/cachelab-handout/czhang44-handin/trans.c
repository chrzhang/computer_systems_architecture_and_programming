/*
CS220 Spring 2014 Lab 4: Cache Lab
Part B: Optimizing Matrix Transpose
Completed by Christopher Zhang (czhang44@binghamton.edu)
*/

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
void transpose_32_32(int M, int N, int A[N][M], int B[M][N]);
void transpose_64_64(int M, int N, int A[N][M], int B[M][N]);
void transpose_61_67(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]) {
    switch (M) {
    case 32:
        transpose_32_32(M, N, A, B);
        break;
    case 64:
        transpose_64_64(M, N, A, B);
        break;
    case 61:
        transpose_61_67(M, N, A, B);
        break;
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 
char transpose_32_32_desc[] = "Transpose the 32 x 32 matrix";
void transpose_32_32(int M, int N, int A[N][M], int B[M][N]) {
    // The sizeof(int) == 4 and the block size is 2^5 or 32 per line,
    // meaning each line can hold 8 ints. Using blocking, the block-size of
    // 8 nicely divides the 32 square matrix test case
    int BLOCK_SIZE;
    int rowIndex; // Index to pass through the rows
    int colIndex; // Index to pass through the columns
    int blockedRowIndex; // Index to pass through the rows in a block
    int blockedColIndex; // Index to pass through the cols in a block
    // Later, realizing that a block's diagonal need not be affected if it is
    // part of the matrix's entire diagonal, use temporary local variables
    // instead of having to re-access those elements
    int eBlockDiagl; // Element of the diagonal
    int iBlockDiagl; // Index of the diagonal (row/column index)
    BLOCK_SIZE = 8;        
    // Even though M == N, both will be used for intuitive clarity
    for (colIndex = 0; colIndex < M; colIndex += BLOCK_SIZE) {
        for (rowIndex = 0; rowIndex < N; rowIndex += BLOCK_SIZE) {
            for (blockedRowIndex = rowIndex; blockedRowIndex < rowIndex +
                 BLOCK_SIZE; ++blockedRowIndex) {
                for (blockedColIndex = colIndex; blockedColIndex < colIndex
                        + BLOCK_SIZE; ++blockedColIndex) {
                    if (blockedRowIndex != blockedColIndex) {
                        B[blockedColIndex][blockedRowIndex] =
                            A[blockedRowIndex][blockedColIndex];
                    }
                    else {
                        eBlockDiagl = A[blockedRowIndex][blockedColIndex];
                        iBlockDiagl = blockedRowIndex;
                    }
                }
                // If it's on the diagonal, refer to the diagonal stored while
                // iterating through each row in the block
                if (colIndex == rowIndex) {
                    B[iBlockDiagl][iBlockDiagl] = eBlockDiagl;
                }                
            }
        }
    } 
}

char transpose_64_64_desc[] = "Transpose the 64 x 64 matrix";
void transpose_64_64(int M, int N, int A[N][M], int B[M][N]) {
    int rowIndex, colIndex;
    int reg1, reg2, reg3, reg4, reg5, reg6, reg7, reg8;
    int ct; // Since only 12 local variables are allowed, 8 is used as a magic #
    for (rowIndex = 0; rowIndex < N; rowIndex += 8) {
        for (colIndex = 0; colIndex < M; colIndex += 8) {
            // Now opearting block-wise
            for (ct = 0; ct < 8; ct++){
                // Utilize the local variables as "registers" to avoid
                // future misses
                // Store the 4 numbers in the current row in A 
                reg1 = A[colIndex + ct][rowIndex];
                reg2 = A[colIndex + ct][rowIndex + 1];
                reg3 = A[colIndex + ct][rowIndex + 2];
                reg4 = A[colIndex + ct][rowIndex + 3];
                if (0 == ct) {
                    // If on the first iteration within a block,
                    // Store the start of each row
                    reg5 = A[colIndex + ct][rowIndex + 4];
                    reg6 = A[colIndex + ct][rowIndex + 5];
                    reg7 = A[colIndex + ct][rowIndex + 6];
                    reg8 = A[colIndex + ct][rowIndex + 7];
                }
                // Now consider the block in B to be transposed into
                // Advancing in 64s (jump across an entire row), swap
                B[rowIndex][colIndex + ct] = reg1;
                B[rowIndex][colIndex + ct + 64] = reg2;
                B[rowIndex][colIndex + ct + 128] = reg3;
                B[rowIndex][colIndex + ct + 192] = reg4;
            }
            // Now go down (note how there's only 7 iterations since there is a
            // corner block shared in row-wise and column-wise iterations
            for (ct = 7; ct > 0; ct--) {
                reg1 = A[colIndex + ct][rowIndex + 4];
                reg2 = A[colIndex + ct][rowIndex + 5];
                reg3 = A[colIndex + ct][rowIndex + 6];
                reg4 = A[colIndex + ct][rowIndex + 7];
                B[rowIndex + 4][colIndex + ct] = reg1;
                B[rowIndex + 4][colIndex + ct + 64] = reg2;
                B[rowIndex + 4][colIndex + ct + 128] = reg3;
                B[rowIndex + 4][colIndex + ct + 192] = reg4;
            }
            // Consider the block in B to be transposed into
            // Advancing in 64s (jump across an entire row), swap
            B[rowIndex + 4][colIndex] = reg5;
            B[rowIndex + 4][colIndex + 64] = reg6;
            B[rowIndex + 4][colIndex + 128] = reg7;
            B[rowIndex + 4][colIndex + 192] = reg8;
        }
    } 
}

char transpose_61_67_desc[] = "Transpose the 61 x 67 matrix";
void transpose_61_67(int M, int N, int A[N][M], int B[M][N]) { 
    int rowIndex, colIndex; // Indices to pass through rows/columns in blocks
    int blockedRowIndex, blockedColIndex; // Indices to step within blocks
    int temp;
    for(rowIndex = 0; rowIndex < N; rowIndex += 16) {
        for(colIndex = 0; colIndex < M; colIndex += 4) {
            // Do not try to transpose outside of the bounds of the matrix
            for(blockedRowIndex = rowIndex; (blockedRowIndex < rowIndex + 16) &&
                (blockedRowIndex < N); ++blockedRowIndex) {
                for(blockedColIndex = colIndex; (blockedColIndex < colIndex + 4)
                    && (blockedColIndex < M); ++blockedColIndex) {
                    // Store diagonals
                    if (blockedRowIndex - rowIndex == 
                        blockedColIndex - colIndex) {
                        temp = A[blockedRowIndex][blockedColIndex];
                    }
                    else { // Do a normal in-block swap
                        B[blockedColIndex][blockedRowIndex] = 
                            A[blockedRowIndex][blockedColIndex];
                    }
                }
                for (blockedColIndex = colIndex; (blockedColIndex < 
                    colIndex + 4) && (blockedColIndex < M); ++blockedColIndex) {
                    // Map the diagonals
                    if (blockedRowIndex - rowIndex == 
                        blockedColIndex - colIndex) {
                        B[blockedColIndex][blockedRowIndex] = temp;
                    }
                }
            }
        }
    } 
}

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple r w-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N]) { 
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
void registerFunctions() {
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 
    /* Register any additional transpose functions */
//  registerTransFunction(trans, trans_desc); 
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N]) {
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
