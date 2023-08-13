
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#define SIZE    100
#define HEAVY 1000000


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

// Sequencial code to be parallelized
int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	int coef = atoi(argv[1]);
	double sum = 0;
	double start_time = MPI_Wtime(); 
	for (int i = 0; i < 100; i++)
		sum += heavy(i, coef, rand());
	double end_time = MPI_Wtime();
	printf("sum = %e\n Execution time = %f\n", sum, end_time - start_time);

	MPI_Finalize();
}
