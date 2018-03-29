#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  // Parallel Version

  int n;      // range of array
  int p_num;  // num of process
  int p_id;   // rank of process

  MPI_Init(&argc, &argv);

  // start timer
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
  // ensure sqrt of n is in the first portion
  if (p_id == 0 && p_num > max_p_num) {
    printf("The amout of processes should not be greater than %d.\n",
           max_p_num);
    MPI_Finalize();
    exit(1);
  }

  // portion parameter
  int low = (p_id * n / p_num + 2);
  int high = ((p_id + 1) * n / p_num + 2) - 1;
  int std_size = high - low + 1;
  high = high > n ? n : high;
  int size = high - low + 1;
  printf("process %d, size %d, [%d, %d]. n=%d p_num=%d\n", p_id, size, low,
         high, n, p_num);

  // allocate and initialize portion
  char* portion = (char*)malloc(size * sizeof(char));
  if (portion == NULL) {
    printf("Memory is full.\n");
    MPI_Finalize();
    exit(1);
  }
  for (int i = 0; i < size; ++i) portion[i] = 1;

  // sieve
  int prime = 2, op_index, root_index = 0;
  do {
    // find the first index to sieve
    if (p_id == 0) {
      op_index = prime * prime - low;
    } else {
      if (!(low % prime))
        op_index = 0;
      else
        op_index = prime - (low % prime);
    }

    // inner-portion sieve
    for (; op_index < size; op_index += prime) {
      portion[op_index] = 0;
      // if (p_id == 0)
      //   printf("p %d set %d to false.\n", p_id, p_id * std_size + 2 + op_index);
    }

    // if (p_id == 0) {
    //     printf("The first portion\n");
    //     for (int i = 0; i < size; ++i) {
    //         printf("[%d: %d]", i + 2, portion[i]);
    //     }
    //     printf("\n");
    //     exit(1);
    // }

    // root process find next prime and broadcast
    if (p_id == 0) {
      root_index++;
      while (portion[root_index] == 0) {
        // printf("%d is not prime.", root_index + 2);
        root_index++;
      }
      prime = root_index + 2;
      // printf("root choose prime %d\n", prime);
    }
    MPI_Bcast(&prime, 1, MPI_INT, 0, MPI_COMM_WORLD);

  } while (prime * prime <= n);

  // count primes
  int count = 0;
  int global_count = 0;
  for (int i = 0; i < size; ++i)
    if (portion[i]) count++;
  // printf("process %d got count %d.\n", p_id, count);
  MPI_Reduce(&count, &global_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  time_usage += MPI_Wtime();

  free(portion);

  if (p_id == 0) {
    printf("-- Parallel Report --\n");
    printf("Time usage: %10.6fs\n", time_usage);
    printf("[2, %d] has %d prime.\n", n, global_count);
  }

  // Sequential Version
  if (p_id == 0) {
    time_usage = -MPI_Wtime();
    char flag[n + 1];
    for (int i = 0; i <= n; ++i) flag[i] = 1;

    flag[0] = 0;
    flag[1] = 0;
    for (int i = 2; i <= n; ++i) {
      if (flag[i])
        for (int j = 2 * i; j <= n; j += i) flag[j] = 0;
    }
    int seq_count = 0;
    for (int i = 0; i <= n; ++i)
      if (flag[i]) seq_count++;

    time_usage += MPI_Wtime();

    printf("-- Sequential Report --\n");
    printf("Time usage: %10.6fs\n", time_usage);
    printf("[2, %d] has %d prime.\n", n, seq_count);
  }

  MPI_Finalize();
}