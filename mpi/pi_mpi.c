/*
    This program calculates the value of pi using Gregory-Leibniz infinite series.
    The program is imlemented using MPI.
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <float.h>


int main(int argc, char *argv[])
{
    int my_rank, comm_size;
    long long n; // number of terms for the series

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    if (my_rank == 0)
    {
        if (argc != 2)
        {
            printf("Usage: mpiexec -n <number of processes> %s <number of terms>", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        n = atoll(argv[1]);
    }

    // broadcast the n to all the other processes in the communicator
    MPI_Bcast(&n, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    // now each process devides their own work
    long long local_n = n / comm_size;
    long long first_index = my_rank * local_n;
    long long last_index = first_index + local_n;

    // if not completely divisible , then the remainder terms will be given to last process
    if (my_rank == comm_size - 1)
    {
        last_index = n;
    }

    long double local_sum = 0.0; // to store the local_sum for each process

    // each processes calculates its local_sum
    for (long long i = first_index; i < last_index; i++)
    {
        long double factor;
        if (i % 2 == 0)
        {
            factor = 1.0;
        }
        else
        {
            factor = -1.0;
        }

        local_sum += factor / (2.0 * i + 1.0);
    }

    long double global_sum = 0.0; // global sum of all the factors

    // process zero collects all the terms from each process and sum is stored in global_sum of process 0
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


    //printing the final result
    if(my_rank == 0){
        long double pi_value = 4.0 * global_sum;
        printf("Using N = %lld terms of the Gregory-Leibniz infinite series;\n",n);
        int limit = LDBL_DIG; // this will give us the maximum precision of our system
        //this will print caluclated value of Pi within the precision limit of the system 
        printf("Estimated Value of Pi : %.*Lf\n",limit,pi_value);  
    }

    MPI_Finalize();
    return 0;
}
