#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"
#define N 10

int main(int argc, char *argv[])
{
    int rank;
    int size;

    MPI_Init(0, 0);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    char* p = malloc(N * sizeof(int));
    strcpy(p, "Anonymous");
    /*
    if (rank == 0)
    {
        strcpy(p, "Anonymous");
        // MPI_Send(p, N, MPI_CHAR, 1, 0, MPI_COMM_WORLD); // 实测也是non-Blocking的 原因不清楚
        MPI_Request req1;
        MPI_Isend(p, N, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &req1); // 实测也non-Blocking的
        printf("Rank %d send\n", rank);
    }
    else if (rank == 1)
    {
        strcpy(p, "Hydra_QYH");
       sleep(5);
        MPI_Recv(p, N, MPI_CHAR, 0, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Rank %d recive\n", rank);
    }
    */
   
   int tmp = rank;
   printf("Rank %d pre-value is %d\n", rank, tmp);
   // Blocking format
   // MPI_Send(&tmp, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD);
   // non-Block format
   MPI_Request send_request;
   MPI_Isend(&tmp, 1, MPI_INT, (rank + 1) % size, 0, MPI_COMM_WORLD, &send_request);
    if (rank == 0)
    {
        // MPI_Recv(&tmp, 1, MPI_INT, size - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Request recv_request;
        MPI_Irecv(&tmp, 1, MPI_INT, size - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_request);
        MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
    }
    else
    {
        // MPI_Recv(&tmp, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Request recv_request;
        MPI_Irecv(&tmp, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &recv_request);
        // MPI_Wait(&recv_request, MPI_STATUS_IGNORE);
        int flag = 0;
        do
        {
           //  sleep(1);
            MPI_Test(&recv_request, &flag, MPI_STATUS_IGNORE);
        } while (!flag);
    }
    printf("Rank %d post-value is %d\n", rank, tmp);
    
    // printf("%s\n", p);
    free(p);
    MPI_Finalize();
    return 0;
}
