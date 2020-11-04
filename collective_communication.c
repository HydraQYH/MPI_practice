#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

void printBuffer(const int* buf, int size);

int main(int argc, char *argv[])
{
    int tasks_pre[4] = {0,1,2,3};
    int tasks_post[4] = {4,5,6,7};
    int rank;
    int size;

    MPI_Init(0, 0);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // MPI_Barrier for synchronization
    // MPI_Barrier(MPI_COMM_WORLD);
    // printf("Rank %d process synchronization\n", rank);

    // MPI_Bcast
    // int* p = (int*)malloc(sizeof(int));
    // *p = rank;
    // printf("Before MPI_Bcast: %d\n", *p);
    // MPI_Bcast(p, 1, MPI_INT, 3, MPI_COMM_WORLD);
    // printf("After MPI_Bcast: %d\n", *p);
    // free(p)

    // MPI_Scatter
    // int* p_send = (int*)malloc(size * sizeof(int));
    // for (int i = 0; i < size; i++)
    // {
    //     if (rank == 5)
    //     {
    //         *(p_send + i) = i;
    //     }
    //     else
    //     {
    //         *(p_send + i) = -1;
    //     }
    // }
    // int* p_recv = (int*)malloc(sizeof(int));
    // *p_recv = -1;
    // printf("Rank %d Before MPI_Scatter: %d\n", rank, *p_recv);
    // MPI_Scatter(p_send, 1, MPI_INT, p_recv, 1, MPI_INT, 5, MPI_COMM_WORLD);
    // printf("Rank %d After MPI_Scatter: %d\n", rank, *p_recv);
    // free(p_recv);
    // free(p_send);

    // MPI_Gather
    // int* p_send = (int*)malloc(sizeof(int));
    // *p_send = rank;
    // int* p_recv = (int*)malloc(size * sizeof(int));
    // printf("Rank %d before MPI_Gather: ", rank);
    // printBuffer(p_recv, size);
    // MPI_Gather(p_send, 1, MPI_INT, p_recv, 1, MPI_INT, 7, MPI_COMM_WORLD);
    // printf("Rank %d After MPI_Gather: ", rank);
    // printBuffer(p_recv, size);
    // free(p_recv);
    // free(p_send);

    // MPI_Reduce
    // int* p_send = (int*)malloc(sizeof(int));
    // *p_send = rank;
    // int* p_recv = (int*)malloc(sizeof(int));
    // printf("Rank %d Before MPI_Reduce: %d\n", rank, *p_recv);
    // MPI_Reduce(p_send, p_recv, 1, MPI_INT, MPI_SUM, 2, MPI_COMM_WORLD);
    // printf("Rank %d After MPI_Reduce: %d\n", rank, *p_recv);
    // free(p_recv);
    // free(p_send);

    // MPI_Allgather
    // int* p_send = (int*)malloc(sizeof(int));
    // *p_send = rank;
    // int * p_recv = (int*)malloc(size * sizeof(int));
    // printf("Rank %d Before Allgather:", rank);
    // printBuffer(p_recv, size);
    // MPI_Allgather(p_send, 1, MPI_INT, p_recv, 1, MPI_INT, MPI_COMM_WORLD);
    // printf("Rank %d After Allgather:", rank);
    // printBuffer(p_recv, size);
    // free(p_recv);
    // free(p_send);

    // MPI_Allreduce
    // int* p_send = (int*)malloc(sizeof(int));
    // *p_send = rank;
    // int* p_recv = (int*)malloc(sizeof(int));
    // printf("Rank %d Before Allreduce: %d\n", rank, *p_recv);
    // MPI_Allreduce(p_send, p_recv, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    // printf("Rank %d After Allreduce: %d\n", rank, *p_recv);
    // free(p_recv);
    // free(p_send);

    // Create New Group
    MPI_Group world_group;
    MPI_Comm_group(MPI_COMM_WORLD, &world_group);
    MPI_Group new_group;
    if (rank < 4)
    {
        MPI_Group_incl(world_group, 4, tasks_pre, &new_group);
    }
    else
    {
        MPI_Group_incl(world_group, 4, tasks_post, &new_group);
    }
    MPI_Comm new_comm;
    MPI_Comm_create_group(MPI_COMM_WORLD, new_group, 0, &new_comm);

    int* p_send = (int*)malloc(sizeof(int));
    *p_send = rank;
    int* p_recv = (int*)malloc(sizeof(int));
    printf("Rank %d Before Allreduce: %d\n", rank, *p_recv);
    MPI_Allreduce(p_send, p_recv, 1, MPI_INT, MPI_SUM, new_comm);
    printf("Rank %d Aftre Allreduce: %d\n", rank, *p_recv);
    free(p_recv);
    free(p_send);
    
    MPI_Finalize();
    return 0;
}

void printBuffer(const int* buf, int size)
{
    printf("{ ");
    for (int i = 0; i < size; i++)
    {
        if (i != size - 1)
        {
           printf("%d, ", *(buf + i));
        }
        else
        {
            printf("%d }\n", *(buf + i));
        }
    }
}
