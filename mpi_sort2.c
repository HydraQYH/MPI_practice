#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include "mpi.h"
#define N 3

void printInfo(int rank, int* p_value);
int cmp(const void *a,const void *b);

int main(int argc, char *argv[])
{
    int rank;
    int size;

    MPI_Init(0, 0);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int value[N];
    int* p_ori = NULL;
    if (rank == 0)
    {
        p_ori = (int*)malloc(size * N * sizeof(int));
        for (int i = 0; i < size * N; i++)
        {
            *(p_ori + i) = size * N - i;
        }
    }
    MPI_Scatter(p_ori, N, MPI_INT, value, N, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        free(p_ori);
        p_ori = NULL;
    }
    printInfo(rank, value);
    qsort(value, N, sizeof(int), cmp);
    int result[N];

    int* p_buffer = (int*)malloc(sizeof(int));
    *p_buffer = INT_MAX;

    int* p_current_task = value;

    for (int i = 0; i < size * N; i++)
    {
        int* p_tmp = (int*)malloc(sizeof(int));
        MPI_Allreduce(p_current_task, p_tmp, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
        if (*p_current_task == *p_tmp)
        {
            if (p_current_task - value == N - 1)
            {
                p_current_task = p_buffer;
            }
            else
            {
                p_current_task++;
            }
        }
        
        if (i >= rank * N && i < (rank + 1) * N)
        {
            result[i - rank * N] = *p_tmp;
        }
        free(p_tmp);
    }

    free(p_ori);
    free(p_buffer);
    
    printInfo(rank, result);
    MPI_Finalize();
    return 0;
}

void printInfo(int rank, int* p_value)
{
    printf("Rank %d values: { ", rank);
    for (int i = 0; i < N; i++)
    {
        if (i != N - 1)
        {
             printf("%d, ", p_value[i]);
        }
        else
        {
             printf("%d }\n ", p_value[i]);
        }
    }
}

int cmp(const void *a,const void *b)
{
    return *(int *)a-*(int *)b;
}
