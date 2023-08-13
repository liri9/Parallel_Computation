
There is one c filesthe docx results file, input file and IPS file.

To compile:
mpicc -fopenmp Ex2.c -o Ex2 -lm


To run with one computer:
mpiexec -np 2 ./Ex2 <num of threads>


To run with two computers:
check the IPs with the command hostname -I
enter them in the IP file that is in the zip folder

and run the command :   mpiexec -np 2 -hostfile IP.txt -map-by node ./Ex2 <num of threads> 

 