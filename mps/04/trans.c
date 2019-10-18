/* 
0;136;0c * trans.c - Matrix transpose B = A^T
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
 */

void trans32(int A[32][32], int B[32][32])
{
  int diag, temp, offset = 8;
  for (int i = 0; i < 32; i += offset)
    {
      for (int j = 0; j < 32; j += offset)
	{
	  for (int n = i; n < i + offset; n++)
	    {
	      for (int m = j; m < j + offset; m++)
		{
		  if (n == m)
		    {
		      diag = n;
		      temp = A[n][m];
		    }
		  else
		    {
		      B[m][n] = A[n][m];

		    }
		}
	      if (i == j)
		{
		  B[diag][diag] = temp;
		}
	    }
	}
    }
}

void trans64(int A[64][64], int B[64][64])
{
  int i, j, k, local[8], temp, temp2;
  for(i = 0; i < 64; i += 8 )
    {
      for(j = 0; j < 64; j += 8)
	{
          for(k = 0; k < 4; k++)
	    {
	      for (temp = 0; temp < 8; temp++)
		{
		  local[temp] = A[i + k][j + temp];
		}

	      for (temp2 = 0; temp2 < 3; temp2++)
		{
		  for (temp = 0; temp < 2; temp++)
		    {
		      B[j + temp2][i + k + (temp * 4)] = local[(temp * 5) + temp2];
		    }
		}
	      
	      B[j + 3][i + k] = local[3];
              B[j + 3][i + k + 4] = local[4];	      
	    }

	  for (temp = 0; temp < 4; temp++)
	    {
	      local[temp] = A[i + temp + 4][j + 4];
	    }

	  for (temp = 4; temp < 8; temp++)
	    {
	      local[temp] = A[i + temp][j + 3];
	    }
	  
	  for (temp = 0; temp < 4; temp++)
	    {
	      B[j + 4][i + temp] = B[j + 3][i + temp + 4];
	      B[j + 4][i + temp + 4] = local[temp];
	      B[j + 3][i + temp + 4] = local[temp + 4];
	    }
	  
          for(k = 0; k < 3; k++)
	    {
	      for (temp = 0; temp < 4; temp++)
		{
		  local[temp] = A[i + temp + 4][j + 5 + k];
		}

	      for (temp = 4; temp < 8; temp++)
		{
		  local[temp] = A[i + temp][j + k];
		}
	      
	      for (temp = 0; temp < 4; temp++)
		{
		  B[j + k + 5][i + temp] = B[j + k][i + temp + 4];
		  B[j + k + 5][i + temp + 4] = local[temp];
		  B[j + k][i + temp + 4] = local[temp + 4];
		}
	    }
        }
    }	
}

void trans61x67(int M, int N, int A[N][M], int B[M][N])
{
  int i, j, diag, temp, offset = 16;
  
  for (i = 0; i < N; i += offset)
    {
      for (j = 0; j < M; j += offset)
        {
          for (int n = i; n < i + offset && n < N; n++)
            {
              for (int m = j; m < j + offset && m < M; m++)
                {
                  if (n == m)
                    {
                      diag = n;
                      temp = A[n][m];
                    }
                  else
                    {
                      B[m][n] = A[n][m];

                    }
                }
              if (i == j)
                {
                  B[diag][diag] = temp;
                }
	    }
        }
    }
}

char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
  if (N == 32)
    {
      trans32(A, B);
    }
  else if (N == 64)
    {
      trans64(A, B);
    }
  else if (N == 61)
    {
      trans61x67(M, N, A, B);
    }
  else if (M == 61)
    {
      trans61x67(M, N, A, B);
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

