#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char** argv) {
    int p_num;
    int p_id;
    int n;

    MPI_Init(&argc, &argv);

    // Start Timer
    MPI_Barrier(MPI_COMM_WORLD);
    double time_usage = -MPI_Wtime();

    MPI_Comm_rank(MPI_COMM_WORLD, &p_id);
    MPI_Comm_size(MPI_COMM_WORLD, &p_num);

    // root process check
    if (p_id == 0 && argc != 2) {
        printf("Command line: %s <m>\n", argv[0]);
        MPI_Finalize();
        exit(1);
    }
    n = atoi(argv[1]);
    int max_p_num = n / (int)(sqrt((double)n) - 1);
    if (p_id == 0 && p_num > max_p_num) {
        printf("The amout of processes should not be greater than %d.\n", max_p_num);
        MPI_Finalize();
        exit(1);
    }

    // portion parameter
    int low = (p_id * n / p_num + 2);
    int high = ((p_id + 1) * n / p_num + 2) - 1;
    high = high > n ? n : high;
    int size = high - low + 1;
    printf("process %d, size %d, [%d, %d]. n=%d p_num=%d\n", p_id, size, low, high, n, p_num);


    // allocate and initialize portion
    char* portion = (char*)malloc(size * sizeof(char));
    if (portion == NULL) {
        printf("Memory is full.\n");
        MPI_Finalize();
        exit(1);
    }
    for (int i = 0; i < size; ++i) portion[i] = 1;



    // // sieve
    // int index;
    // if (p_id == 0) index = 0;
    // int prime = 2;

    // // count primes
    // int count = 0;
    // int global_count = 0;
    // for (int i = 0; i < size; ++i)
    //     if (portion[i]) count++;
    // MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    time_usage += MPI_Wtime();

    free(portion);

    if (p_id == 0) {
        printf("Time usage: %10.6fs/n", time_usage);
    }

    MPI_Finalize();
}