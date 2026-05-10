# <b>Parallel Pi Estomator</b>

A high-performance implementation designed to estimate the value of $\pi$ using parallel computing techniques. This project demonstrates how computational tasks can be partitioned across multiple threads or processes to achieve significant speedup in numerical integration.

## <b>About</b>
Estimating $\pi$ via numerical integration typically using the Leibniz formula or the Monte Carlo method is a classic problem in parallel computing. This repository explores the efficiency of distributed computation by minimizing the execution time required to reach a specific precision level.

This repository contains three different parallel programming implementations to estimate the value of $\pi$ using the Gregory-Leibniz infinite series.

The project serves as a comparative study of parallel computing techniques, demonstrating how computational workloads can be distributed across multiple threads or processes using Pthreads, OpenMP, and MPI.

## <b>Mathematical Foundation</b>

The programs estimate $\pi$ based on the Gregory-Leibniz series, which states that:
$$\frac{\pi}{4} = 1 - \frac{1}{3} + \frac{1}{5} - \frac{1}{7} + \frac{1}{9} - \dots$$
Or, expressed as a summation:
$$\pi = 4 \sum_{i=0}^{\infty} \frac{(-1)^i}{2i+1}$$

By computing this summation up to a very large number of terms ($N$), we can approximate $\pi$. Because each term in the series can be calculated independently, this problem is highly parallelizable. The implementations use long double precision (LDBL_DIG) to output the result to the maximum precision supported by the system.

## <b>Repository Structure</b>
```text
parallel-pi-estimator/
├── mpi/             # Message Passing Interface (MPI) implementation
├── openmp/          # OpenMP implementation
├── pthreads/        # POSIX Threads implementation
├── .vscode/         # Editor configurations
└── .gitignore       # Git ignore rules
```

## <b>Implementations</b>

### <b>1. Pthreads (<code>pthreads/</code>)</b>

This implementation uses POSIX threads to achieve shared-memory parallelism.   

- <b>Workload Division:</b> The total number of iterations $N$ is manually divided by the number of threads. Any remainder terms are assigned to the last thread.
- <b>Synchronization:</b> A <code>pthread_mutex_t</code> lock is used to protect the <code>global_sum variable</code>, ensuring that only one thread can update the total sum at a time after finishing its local calculations.

### <b>2. OpenMp (<code>openmp/</code>)</b>
This version uses OpenMP compiler directives for a much cleaner shared-memory parallelization.

- <b>Workload Division:</b> The <code>#pragma omp parallel for</code> directive automatically partitions the loop iterations among the available threads.
- <b>Synchronization:</b> The <code>reduction(+ : sum)</code> clause is used to handle the local summations and safely accumulate them into the global <code>sum</code> variable without the need for manual mutex locks.

### <b>3. MPI (<code>mpi/</code>)</b>
This implementation uses the Message Passing Interface for distributed-memory parallelism, making it suitable for computing clusters.

- <b>Workload Division:</b> The master process (Rank 0) reads the input and broadcasts the total terms $N$ to all other processes using <code>MPI_Bcast</code>. Each process calculates its own start and end indices.

- <b>Synchronization:</b> Once each process finishes computing its <code>local_sum</code>, <code>MPI_Reduce</code> is used to collect and sum all local values into the <code>global_sum</code> on the master process.


## <b>Prerequisites</b>
To compile and run these programs, you will need:

- A C compiler (e.g., GCC)
- POSIX Threads library (standard on Linux/macOS)
- OpenMP support (standard with modern GCC)
- An MPI implementation (e.g., OpenMPI or MPICH)

## <b>Compilation and Usage</b>
Navigate into the respective directories to compile and run the programs.

### <b>Pthreads</b>
```bash
# Compile
gcc -g -Wall pi_pthreads.c -o pi_pthreads -pthread

# Run: ./pi_pthreads <number_of_threads> <number_of_terms>
./pi_pthreads 4 1000000000
```

### <b>OpenMP</b>
```bash
# Compile
gcc -g -Wall pi_openmp.c -o pi_openmp -fopenmp

# Run: ./pi_openmp <number_of_threads> <number_of_terms>
./pi_openmp 4 1000000000
```

### <b>MPI</b>
```bash
# Compile
mpicc -g -Wall pi_mpi.c -o pi_mpi

# Run: mpiexec -n <number_of_processes> ./pi_mpi <number_of_terms>
mpiexec -n 4 ./pi_mpi 1000000000
```