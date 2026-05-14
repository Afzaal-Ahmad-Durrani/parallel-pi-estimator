/*
    Problem Statement
        Write a parallel computer program to calculate the value of PI.
        The program should accept two arguments on command line. The first argument is number of terms
        in series to add and second argument number of blocks (for CUDA).
        Finally Display the calculated value of PI on console.
*/

/*
    =======MY SOLUTION=======
    -   User provides the number of terms of the series and number of blocks.
    -   Calculate the number of threads for each block.
    -   Each thread calculates a term of the series and add it to
        the global sum. For sumation i use the atomicSum Function
        which saftly adds the terms in global sum without any race condition between threads.
    -   The final global sum is sentto the CPU.

    -   Compilation : nvcc pi-cuda.cu -o pi_cuda -arch=sm_60
    -   Usage: pi_cuda <number of blocks> <number of terms of series>
*/


#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <cuda.h>

// this kernel fucntion will be executed by the GPU
__global__ void piKernel(long long n, double* global_sum)
{
    // index of the current thread. I am using 1D configuartion for both blocks and threads
    long long i = (long long)blockIdx.x * blockDim.x + threadIdx.x;

    if(i < n)
    {
        // calculated the factor
        double factor;
        if (i % 2 == 0)
        {
            factor = 1.0;
        }
        else
        {
            factor = -1.0;
        }
        // calcute the sum
        double term = factor / (2.0 * i + 1.0);

        /*
            My first implementation was to create a global array and store the partial sum
            of thread in that array. then the array was sent to the CPU. The CPU will add
            the data in the partial sum array to calculate the global sum but then i
            found an efficient way that automatically performs that addition in GPU
            (i-e atomicAdd function).
            During compilation we must specify the flag "-arch=sm_60"
        */

        // adds the term into the global sum variable
        //as the compute capability of our system is greater than 6.0 we can saftly use the atomicAdd function
        atomicAdd(global_sum, term); // this function works on GPU with Compute capability gretaer than 6.0
    }
}

int main(int argc, char* argv[])
{
    // if wrong arguments are passed to the program
    if (argc != 3)
    {
        printf("Error in program arugements");
        printf("Usage: %s <block_count> <n>\n", argv[0]);
        return 1;
    }

    // number of blocks
    int block_count = atoi(argv[1]);
    long long n = atoll(argv[2]); // using long long for very large N

    // thread counts per block
    long long thread_per_block = (n + block_count - 1) / block_count;


    // as the maximum number of threads per block is 1024, so
    if(thread_per_block > 1024)
    {
        long long blocks = (n) / 1024;
        printf("Error!!! Cannot have more that 1024 threads per block.\nIncrease the number of Blocks!\n");
        printf("Number of blocks should be greather than %lld for this matrix mutilplication\n",blocks);
        return 1;
    }

    // global sum on cpu
    double global_sum = 0.0;
    // global sum on GPU
    double* global_sum_d;

    cudaError_t error; // error handling during mem allocation on gpu
    error = cudaMalloc((void**)&global_sum_d, sizeof(double));
    if(error != cudaSuccess)
    {
        printf("Malloc Error in global_sum_d : %s\n",cudaGetErrorString(error));
        return 1;
    }

    // copy global sum from CPU to GPU
    cudaMemcpy(global_sum_d, &global_sum, sizeof(double), cudaMemcpyHostToDevice);

    // launching the kernel
    piKernel<<<block_count, thread_per_block>>>(n, global_sum_d);
    cudaDeviceSynchronize();

    // Copy Global sum from GPU to CPU
    cudaMemcpy(&global_sum, global_sum_d, sizeof(double), cudaMemcpyDeviceToHost);

    // calculating the final value of pi
    double pi = 4.0 * global_sum;
    printf("Using N = %lld terms of the Gregory-Leibniz infinite series;\n", n);
    printf("Estimated Value of Pi : %.*f\n", DBL_DIG, pi); // The DBL_DIG prints the value upto the maximum precion limits of the system

    //cleaninh the memory allocated on the GPU
    cudaFree(global_sum_d);
    return 0;
}
