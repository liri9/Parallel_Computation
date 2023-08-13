

// to run the program from terminal  :
// mpicc -o dynamic dynamic.c -lm
// mpiexec -np 4 ./dynamic 
// ( number of slaves for the program to run if we want 4 slave we will write 5 - the first one is the master



#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define HEAVY 1000000
#define SIZE 100
#define TERMINATION_TAG 0
#define WORK_TAG 1

// This function performs heavy computations,
// its run time depends on a, b, number
// DO NOT change this function
double heavy(int a, int b, int number) {
    int i, loop;
    double sum = 0;
    loop = HEAVY * (number % b);
    for (i = 0; i < loop; i++)
        sum += sin(a*exp(cos((double)(i%5))));
    return  sum;
}

int main(int argc, char **argv)
{
	
	int coef = atoi(argv[1]);
    int size = SIZE;
    int taskId = 0;
    double taskResult = 0;
    double result = 0;	
    int rank, numOfProcs;
    double startTime, endTime;
    MPI_Status status;
    
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);

    if (numOfProcs < 2 || numOfProcs > 9)
	{
		printf("Can not run the program with %d processes\n", numOfProcs);
		MPI_Abort(MPI_COMM_WORLD,0);
	}
	
	
	// Master 
    if (rank == 0) {
    
    	printf("Starting dynamic task pool with %d slaves\n", numOfProcs - 1);
	startTime = MPI_Wtime();
        
        int source;
        
        
        for (int i = 1; i < numOfProcs; i++) {
            MPI_Send(&taskId, 1, MPI_INT, i, WORK_TAG, MPI_COMM_WORLD);
            taskId++;
        }
        
        // Receive from the salve
        
        
        while (taskId < size) {
        
            MPI_Recv(&taskResult, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            source = status.MPI_SOURCE;
            result += taskResult;
            MPI_Send(&taskId, 1, MPI_INT, source, WORK_TAG, MPI_COMM_WORLD);
            taskId++;
        }
        // Receive remaining results from slave
        for (int i = 1; i < numOfProcs; i++) {
            MPI_Recv(&taskResult, 1, MPI_DOUBLE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            result += taskResult;
        }
        // Send termination signal to slave
        for (int i = 1; i < numOfProcs; i++) {
            MPI_Send(&taskId, 0, MPI_INT, i, TERMINATION_TAG, MPI_COMM_WORLD);
        }
        
        
        endTime = MPI_Wtime();	
	double time = endTime - startTime;

        printf("Number of slaves: %d\nExcute time: %lf\nResult: %e\n", numOfProcs - 1, time, result);
        
        
     // slave
    } else {
        
        int taskId;
        MPI_Status status;
        int last = 0;
        
        
        
        
        while (1) {
        
            MPI_Recv(&taskId, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == TERMINATION_TAG) {
                break;
            }
            for (int i = last; i < taskId; i++)       
                rand();
                
            last = taskId +1 ;
            
            taskResult = heavy(taskId, coef, rand());
            MPI_Send(&taskResult, 1, MPI_DOUBLE, 0, taskId, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;
}







