#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <mpi.h>

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))
#define ERROR_TRESH 0.01

double **A;
double **AOrig; 
double **L;
double **U;
double **P;
int N, B;
int maxRowIndex;
struct timeval start, end;

int getBlockSize(int N)
{
  //TODO - Write implemementation to determine block size or make it a 
  //parameter that is passed by the user.
  B = 100;
  return B;  
}

void initialize()
{
  int i, j, k, ii;

  A = malloc((N+1) * sizeof *A);   
  AOrig = malloc(N * sizeof *AOrig);
  L = malloc(N * sizeof *L);
  U = malloc(N * sizeof *U);

  for(i=0; i<N; i++)
  {
    A[i] = malloc((N+1) * sizeof *A[i]);
    AOrig[i] = malloc((N+1) * sizeof * AOrig[i]);
    L[i] = malloc((N+1) * sizeof *L[i]);
    U[i] = malloc((N+1) * sizeof *U[i]);
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
  int i, j, k, ii;

  printf("Matrix %c:\n", name);
  double arrCopy [N][N];

  for(i=0; i<N; i++)
  {
    for(j=0; j<N; j++)
    {
      printf("%.3f ", matrix[i][j]);
    }
    
    printf("\n");
  }   
  printf("\n");
}

int findMaxRowIndex(int k, int max)
{
  int i, j, ii;
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

void swapRows(double **matrix, int row1, int row2)
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
  int i,j,k,ii;
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

void algorithm()
{
  //Make note of the start time
  gettimeofday(&start, 0);

  //Perform LU decomposition
  int mpi_rank, mpi_size;

  // Initialize MPI after the timer has started.
  // The threads split here after all and the initialization is
  // part of the overhead.
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

  //Perform LU decomposition
  int i,j,k,ii;
  int min;
  int rowsToSend;
  int bcaster = 0;

  
  // Getting all pivots that have entries below them (N - 1)
  for(k=0; k<N-1; k++)
  {
	// Here, we are acting on a single column (of index k)
       	for(i=k+1; i<N; i++)
	{
		// Update the entries below the pivot at A[k][k].
        	A[i][k] = A[i][k]/A[k][k];
	}

    //We broadcast the updated matrix A to all processes. We do it here instead
    //of at the bottom because only the root will have to broadcast it here instead
    //of the individual processes.
    MPI_Bcast(&A[0][0], N*N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for(i=k+1; i<N; i=i+B)
	{                             
        min = (i+B) < N ? i + B : N;
        rowsToSend = (i+B) < N ? B : N-i;
        bcaster = ((int)(floor(i/B)) % mpi_size); 
   		
        if(((int)(floor(i/B)) % mpi_size) == mpi_rank)
		{
           for(ii=i; ii<min; ii++)
           {
             for(j=k+1; j<N; j++)
             {
               A[ii][j] = A[ii][j] - (A[ii][k]*A[k][j]);
             }
           }
        }

        //After each process is done updating the block they were assigned, the execute a gather 
        //so the root process will receive all of the updates. 
        MPI_Gather(&A[i][0], rowsToSend*(N), MPI_DOUBLE, &A[i][0], rowsToSend*(N), MPI_DOUBLE, 0, MPI_COMM_WORLD);
     }

  }

  MPI_Finalize();

  gettimeofday(&end, 0);

  if(mpi_rank == 0)
    return;

  else
    exit(0);
}

int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    printf("Usage: ./output_file matrix_size block_size\n");
    return 0;
  }

  //Set the matrix and block sizes.
  N = atoi(argv[1]);
  B = atoi(argv[2]);
  
  //Initialize A, L, U matrices
  initialize();

  // Run the LU algorithm
  algorithm(); 
 
  // Getting the L and U matricies back is done by simply analyzing the 
  // computed A matrix. This is done outside the decomposition since
  // it has nothing to do with the actual algorithm.
  // TEST
  //print('A', A);

  //Check if LU decomposition is valid. This is commented when trying to note 
  //time it takes program to run as the testing seems to take really long.
  /*if(testPassed() == 1){
    printf("LU decomposition is valid\n\n");
  }else{
    printf("LU decomposition is not valid\n\n");
  }*/

  //Output the time required to perform the decomposition
  unsigned long t = (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec);
  printf("Time of solve is: %ld us\n", t);  
 
  //Free all the initalized arrays.
  free(A);
  free(AOrig);
  free(L);
  free(U);
}
