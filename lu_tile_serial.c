#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

double **A;
double **L;
double **U;
double **P;
int N, B;
int i, j, k, ii, maxRowIndex;

int getBlockSize(int N)
{
  //TODO - Write implemementation to determine block size.
  B = 64;
  return B;  
}

void initialize()
{
  A = malloc(N * sizeof *A);   
  L = malloc(N * sizeof *L);
  U = malloc(N * sizeof *U);
  P = malloc(N * sizeof *P);

  for(i=0; i<N; i++)
  {
    A[i] = malloc(N * sizeof *A[i]);
    L[i] = malloc(N * sizeof *L[i]);
    U[i] = malloc(N * sizeof *U[i]);
    P[i] = malloc(N * sizeof *P[i]);
  }

  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      A[i][j] = A[i][0] * j + 1;
      U[i][j] = 0;//A[i][j];

      if(i==j)
      {
        L[i][j] = 1;
        P[i][j] = 1;
      }
      else
      { 
        L[i][j] = 0;
        P[i][j] = 0;
      }

    }
  }
}

void print(char name, double **matrix)
{
  printf("Matrix %c:\n", name);
  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      printf("%.3f ", matrix[i][j]);
    }
    
    printf("\n");
  }   
}

int findMaxRowIndex(int k, int max)
{
  int maxIndex = k;
  int maxSoFar = max;

  for(i=k; i<N; i++)
  {
    if(A[i][k] > max)
    {
      maxSoFar = A[i][k];
      maxIndex = i;
    }
  }

  return maxIndex;
}

int swapRows(double **matrix, int row1, int row2)
{
  double *tmp = matrix[row1];
  matrix[row1] = matrix[row2];
  matrix[row2] = tmp;
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Usage: ./output_file matrix_size");
    return 0;
  }

  N = atoi(argv[1]);
  B = getBlockSize(N);
  initialize();

  printf("Array size = %d\n", N);
  printf("Done initializing\n");
  printf("Block size = %d\n", B);

  A[0][0] = 2;
  A[0][1] = 3;
  A[0][2] = 0;
  A[1][0] = -1;
  A[1][1] = 2;
  A[1][2] = -1;
  A[2][0] = 3;
  A[2][1] = 0;
  A[2][2] = 3; 
  for(i=0; i<3; i++){
   for(j=0; j<3; j++){
     U[i][j] = 0;//A[i][j];
   }
  }

  printf("\n\n");
  for(k=0; k<N; k++)
  {
    maxRowIndex = findMaxRowIndex(k, A[k][k]); 
    printf("K=%d\n",k);
    /*if(maxRowIndex != k){
      printf("Swapping rows: %d , %d\n", k, maxRowIndex);
      swapRows(P, k, maxRowIndex);
      swapRows(A, k, maxRowIndex);
    }*/

    for(i=k; i<N; i++)
    {
      U[k][i] = A[k][i];
    }

    for(i=k+1; i<N; i++)
    { 
      if(k==0 && i==1){
        printf("A[1][0]/A[0][0]=%f\n", A[i][k]/A[k][k]);
      }
      L[i][k] = A[i][k]/A[k][k];
    }

    for(ii=k+1; ii<N; ii++)
    {
      for(j=k+1; j<N; j++)
      {
        A[ii][j] = A[ii][j]-(L[ii][k]*U[k][j]);
      }
    }

    print('A', A);
    print('U', U);
    print('L', L);
    print('P', P);
    printf("\n");
  }
}
