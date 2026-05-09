/*
    I will be following the same logic as i implemented in the pthread.
    The program is implemented using OpenMP.
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <float.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <thread_count> <n>\n", argv[0]);
        return 1;
    }

    int thread_count = atoi(argv[1]);
    long long n = atoll(argv[2]); // using long long for very large N
    long double sum = 0.0;

// parallelizing the for loop using OMP
    #pragma omp parallel for num_threads(thread_count) reduction(+ : sum)
    for (long long i = 0; i < n; i++)
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
        long double local_sum = factor / (2.0 * i + 1.0);
        sum += local_sum;
    }

    // estimated pi value
    long double pi_value = 4.0 * sum;
    int limit = LDBL_DIG;// this will give us the maximum precision of our system
    printf("Using N = %lld terms of the Gregory-Leibniz infinite series;\n", n);
    //this will print caluclated value of Pi within the precision limit of the system 
    printf("Estimated Value of Pi : %.*Lf\n", limit, pi_value);
    return 0;
}