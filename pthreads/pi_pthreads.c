#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <float.h>

// Shared variables
long long n;
long thread_count;
long double global_sum = 0.0;

pthread_mutex_t mutex;

// this function calculates the value of Pi
void *calculate_pi(void *args)
{
    long my_rank = (long)args;

    // now we divide the workload between the threads
    long long local_n = n / thread_count;
    long long first_index = my_rank * local_n;
    long long last_index = first_index + local_n;

    // if the n is not divisible by th enumber of threads
    // remaining Ns are handed to the last thread
    if (my_rank == thread_count - 1)
    {
        last_index = n;
    }

    long double local_sum = 0.0;
    // this is the main series that will be used to calculate the value of Pi
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

    // Lock the mutex before adding local_sum to the global_sum
    pthread_mutex_lock(&mutex);
    global_sum += local_sum;
    pthread_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
    // the signature does'nt match
    if (argc != 3)
    {
        printf("Usage: %s <thread_count> <n>\n", argv[0]);
        return 1;
    }

    thread_count = atoi(argv[1]);
    n = atoll(argv[2]);

    pthread_t *thread_handles = malloc(thread_count * sizeof(pthread_t));
    pthread_mutex_init(&mutex, NULL);

    for (long long i = 0; i < thread_count; i++)
    {
        pthread_create(&thread_handles[i], NULL, calculate_pi, (void *)i);
    }

    for (long long i = 0; i < thread_count; i++)
    {
        pthread_join(thread_handles[i], NULL);
    }

    long double pi = 4.0 * global_sum;

    printf("Using N = %Ld terms of the Gregory-Leibniz infinite series;\n", n);
    int limit = LDBL_DIG; // this will give us the maximum precision of our system
    // this will print caluclated value of Pi within the precision limit of the system
    printf("Estimated Value of Pi : %.*LF\n", limit, pi);

    return 0;
}