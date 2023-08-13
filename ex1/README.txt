
There are three c files and the docx resultrs file 
To compile:
mpicc original.c -o original -lm
mpicc static.c -o static -lm
mpicc dynamic.c -o dynamic -lm

To run the static or dynamic:
mpiexec -np <number of procceses (3/5/9)> ./<name_of_program_to_run> <coefficient>

To run the original code:
mpiexec ./<name_of_program_to_run> <coefficient>