/*
  ron zorea - 316014729
  liri lichow - 206540759
  
  for run the program from terminal: 
  mpicc -fopenmp Ex2.c -o Ex2 -lm
  mpiexec -np 2 ./Ex2 2
  
  for run from 2 computers :
  chceck IP for the IP file : hostname -I
  mpiexec -np 2 -hostfile IP.txt -map-by node ./Ex2 2 
  
 */
 


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <omp.h>
#define FILENAME "input.txt"

double f(double a, int b) {
    double value = 0;
    for (int i = 0; i < 1000000; i++) {
        value += sin(a*sin(b*cos(a*b)));
    }
    return value;
}

void readfile(const char* fileName ,double **A, int *size) {
    FILE *fp = fopen(fileName, "r");
    if (fp == NULL) 
    {
        printf("Can't open file %s\n", fileName);
        MPI_Abort(MPI_COMM_WORLD, 1);
        
    }
    // read the array size from first row in file
    fscanf(fp, "%d", size);
    *A = (double *)malloc(*size * sizeof(double));
    if (*A == NULL) {
        printf("can't allocate memory for A\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        
    }
    // read the array values from the file
    for (int i = 0; i < *size; i++) {
        fscanf(fp, "%lf", &(*A)[i]);
    }
    fclose(fp);
    
}





int main(int argc, char **argv) {

	int numOfThreads = atoi(argv[1]);
    int rank, numOfProcs;
    int arraySize;
    double *A = NULL;
    double *B;
    double totalSum = 0.0;
    double localSum = 0.0;
    double startTime, endTime, totalTime;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);
    startTime = MPI_Wtime();

    if (rank == 0) {
        readfile(FILENAME, &A, &arraySize);
    }


    // Broadcast arraySize to all processes
    MPI_Bcast(&arraySize, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute local chunk size
    int staticSize = arraySize / numOfProcs;
    int remainder = arraySize % numOfProcs;
    int localSize = staticSize + (rank < remainder ? 1 : 0);

    // Allocate memory for B
    B = (double *)malloc(localSize * sizeof(double));
    if (B == NULL) {
        printf("Error: could not allocate memory for B\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Scatter A to all processes
    int *numberOfElements = (int *)malloc(numOfProcs * sizeof(int));
    int *startingIndex = (int *)malloc(numOfProcs * sizeof(int));
    if (numberOfElements == NULL || startingIndex == NULL) {
        printf("can not allocate memory for counts and displs\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    for (int i = 0; i < numOfProcs; i++) {
        numberOfElements[i] = staticSize + (i < remainder ? 1 : 0);
        startingIndex[i] = i * staticSize + (i < remainder ? i : remainder);
    }
    MPI_Scatterv(A, numberOfElements, startingIndex, MPI_DOUBLE, B, localSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);


	omp_set_num_threads(numOfThreads);
	
    // Compute local sum

     #pragma omp parallel for reduction(+:localSum)
    for (int i = 0; i < localSize; i++) {
        localSum += f(B[i], i + startingIndex[rank]);
    }
    
    
    // Reduce local sums to total sum
    MPI_Reduce(&localSum, &totalSum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	
	
	endTime = MPI_Wtime();
	totalTime = endTime - startTime;
    // Print result
    if (rank == 0) {
        printf("\nThe sum of the values in array B: %f\n", totalSum);
        printf("The exec time: %lf\n", totalTime);
    }

    // Cleanup
    if (A != NULL){
     free(A);
     }
   	
  MPI_Finalize();
    return 0;
}

