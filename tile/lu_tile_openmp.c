#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define ERROR_TRESH 0.01

double **A;
double **AOrig; 
double **L;
double **U;
double **P;
int N, B;
int i, j, k, ii, maxRowIndex;
struct timeval start, end;

int getBlockSize(int N)
{
  //TODO - Write implemementation to determine block size.
  B = 10000000;
  return B;  
}

void initialize()
{
  A = malloc(N * sizeof *A);   
  AOrig = malloc(N * sizeof *AOrig);
  L = malloc(N * sizeof *L);
  U = malloc(N * sizeof *U);

  for(i=0; i<N; i++)
  {
    A[i] = malloc(N * sizeof *A[i]);
    AOrig[i] = malloc(N * sizeof * AOrig[i]);
    L[i] = malloc(N * sizeof *L[i]);
    U[i] = malloc(N * sizeof *U[i]);
  }

  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      A[i][j] = (i+1)*(j+1);
      AOrig[i][j] = (i+1)*(j+1);
      U[i][j] = 0;

      if(i==j)
      {
        L[i][j] = 1;
        A[i][j] *= 10;
        AOrig[i][j] *= 10;
      }
      else
      { 
        L[i][j] = 0;
      }
    }
  }
}

void print(char name, double **matrix)
{
  printf("Matrix %c:\n", name);
  double arrCopy [N][N];

  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      printf("%.10f ", matrix[i][j]);
    }
    
    printf("\n");
  }   
  printf("\n");
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

/*If partial pivoting is not used, there will be a lot
of divisions by small numbers that occur. Thus, in the end,
when we multiply L*U, if the value for each entry in the
matrix is equal to it's corresponding entry in the A array
withing a certain treshold, the factorization is considered
to be successful*/
int testPassed()
{
  double sum;

  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      sum = 0;
      for(k=0; k<N; k++)
      {
        sum += L[i][k]*U[k][j];
      }
      if(abs(AOrig[i][j] - sum) > ERROR_TRESH)
      {
        printf("Failing case:\n");
        printf("A[%d][%d]=%0.3f\n",i,j,AOrig[i][j]);
        printf("sum=%0.3f\n",sum);
        return 0;
      }
    }
  }
  return 1;
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Usage: ./output_file matrix_size");
    return 0;
  }

  //Set the matrix and block sizes.
  N = atoi(argv[1]);
  B = getBlockSize(N);
  
  //Initialize A, L, U matrices
  initialize();
  
  //Output the A matrix
  //print('A', A);

  printf("Timer started\n");
  //Make note of the start time
  gettimeofday(&start, 0);

  //Perform LU decomposition
  for(k=0; k<N; k++)
  {
    for(i=k; i<N; i++)
    {
        U[k][i] = A[k][i];
    }

    for(i=k+1; i<N; i++)
    { 
      L[i][k] = A[i][k]/A[k][k];
    }
      #pragma omp parallel for private (i, ii, j) schedule(static)
      for(i=k+1; i<N; i+=B){
        for(ii=i; ii<MIN(ii+B,N); ii++)
        { 
          #pragma omp parallel for private(i, ii, j) schedule(static)
          for(j=k+1; j<N; j++)
          {
            A[ii][j] = A[ii][j]-(L[ii][k]*U[k][j]);
          }
     /* printf("k=%d\n",k);
      print('A', A); */
      //print('L', L);
      //print('U', U);
        }
      }
  }

  //Make note of the end time
  //printf("Done decomposition\n");
  gettimeofday(&end, 0);

  //Check if LU decomposition is valid
  /*if(testPassed() == 1){
    printf("LU decomposition is valid\n\n");
  }else{
    printf("LU decomposition is not valid\n\n");
  }*/

  //Output the L and U matrices
  //print('A', A);
  //print('L', L);
  //print('U', U);

  //Output the time required to perform the decomposition
  unsigned long t = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
  printf("Time of solve is: \n%ld us\n", t);  
 
  //Free all the initalized arrays.
  free(A);
  free(AOrig);
  free(L);
  free(U);
}
