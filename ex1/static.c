

// to run the program from terminal  :
// mpicc -o static static.c -lm
// mpirun -np 4 ./static
// ( number of slaves for the program to run if we want 4 slave we will write 5 - the first one is the master



#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#define SIZE    100
#define HEAVY 1000000


int main(int argc, char **argv);
double heavy(int a, int b,int number);


// This function performs heavy computations, 
// its run time depends on a and b values
// DO NOT change this function
double heavy(int a, int b,int number) {
	int i, loop;
	double sum = 0;
	loop = HEAVY * (number % b);
	for (i = 0; i < loop; i++)
		sum += sin(a*exp(cos((double)(i%5))));
	return  sum;
}

// Sequencial code to be parallelized
int main(int argc, char **argv)
{
    int coef = atoi(argv[1]);
 //   int size = SIZE;
    double result = 0;
    int rank;
    int numOfProcs;
    MPI_Status status;
    double startTime, endTime;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcs);

    if (numOfProcs < 2 || numOfProcs > 9)
    {
        printf("Can not run the program with %d processes\n", numOfProcs);
        MPI_Abort(MPI_COMM_WORLD,0);
    }

    int staticSize = SIZE / (numOfProcs - 1);
    int remainder = SIZE % (numOfProcs - 1);
    
    
    //master 
    if (rank == 0) 
    {
        printf("Starting static task pool with %d slaves\n", numOfProcs - 1);
        startTime = MPI_Wtime();
        result = 0;

        // Send to slave
        for (int p = 0; p < numOfProcs - 1; p++)
        {
            int k = staticSize * p;
            if (p == numOfProcs - 2) {
                k += remainder;
            }
            MPI_Send(&k, 1, MPI_INT, p + 1, 0, MPI_COMM_WORLD);
        }
        // Receive from the salve

        for (int p = 1; p < numOfProcs; p++)
        {
            double temp = 0;

            MPI_Recv(&temp, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

            //sum up the results 
            result += temp;

        }

        endTime = MPI_Wtime();

        double time = endTime - startTime;

        printf("Number of slaves: %d\nExcute time: %lf\nResult: %e\n", numOfProcs - 1, time, result);

    }

    //slave
    else
    {

        result = 0;
        int start = staticSize * (rank - 1);
    	int end = staticSize * rank;
    	for (int i = 0; i < start; i++)  
    	{
        rand();
    	} 
    	
    	if (rank == numOfProcs - 1) {
        end += remainder;
    }

    	
        for (int y = start; y < end; y++){
            result += heavy(y, coef, rand());
        }
        
        //send the results to master
        MPI_Send(&result, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    MPI_Finalize();
    return 0;
}











