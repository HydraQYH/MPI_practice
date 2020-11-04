#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#define N 5

void printInfo(int rank, int* p_value);
void swap(int* p, int * q);

int main(int argc, char *argv[])
{
    int rank;
    int size;

    MPI_Init(0, 0);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // 创建原始数据
    int value[N];
    int total_value = size * N;
    int start_value = total_value - 1 - rank * N;
    for (int i = 0; i < N; i++)
    {
        value[i] = start_value;
        start_value--;
    }
    printInfo(rank, value);
    int sorted = 0;
    while (!sorted)
    {
        sorted = 1;
        if (N % 2 == 0)
        {
            // N为偶数 则even pair时不需要任何通信
            for (int i = 0; i < N; i+=2)
            {
                if (value[i] > value[i + 1])
                {
                    swap(value + i, value + i + 1);
                    sorted = 0;
                }
            }
            // odd pair
             for (int i = 1; i < N - 1; i+=2)
            {
                // 先交换中间不需要通信的部分
                 if (value[i] > value[i + 1])
                {
                    swap(value + i, value + i + 1);
                    sorted = 0;
                }
            }
            // 做边界部分的通信
            if (rank != 0 && rank != size - 1)
            {
                int* p_recv_head = (int*)malloc(sizeof(int));
                int* p_recv_tail = (int*)malloc(sizeof(int));
                // 向rank - 1发送头部值 向rank + 1发送尾部值
                MPI_Request send_request_rank_last;
                MPI_Request send_request_rank_next;

                MPI_Isend(value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &send_request_rank_last);
                MPI_Isend(value + N - 1, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send_request_rank_next);

                // 接收rank - 1和rank + 1发送的头部值和尾部值
                MPI_Recv(p_recv_head, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(p_recv_tail, 1, MPI_INT, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                if (*p_recv_head > value[0])
                {
                    swap(p_recv_head, value);
                    sorted = 0;
                }
                
                if (value[N - 1] > *p_recv_tail)
                {
                    swap(value + N - 1, p_recv_tail);
                    sorted = 0;
                }
                free(p_recv_head);
                free(p_recv_tail);
            }
            else if (rank == 0)
            {
                int* p_recv_tail = (int*)malloc(sizeof(int));
                MPI_Request send_request_rank_next;

                MPI_Isend(value + N - 1, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send_request_rank_next);
                MPI_Recv(p_recv_tail, 1, MPI_INT, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                if (value[N - 1] > *p_recv_tail)
                {
                    swap(value + N - 1, p_recv_tail);
                    sorted = 0;
                }
                free(p_recv_tail);
            }
            else
            {
                // rank == size - 1
                int* p_recv_head = (int*)malloc(sizeof(int));
                MPI_Request send_request_rank_last;

                MPI_Isend(value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &send_request_rank_last);
                MPI_Recv(p_recv_head, 1, MPI_INT, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                
                if (*p_recv_head > value[0])
                {
                    swap(p_recv_head, value);
                    sorted = 0;
                }
                free(p_recv_head);
            }
            // 同步一下flag标志位
            int tmp = 0;
            MPI_Allreduce(&sorted, &tmp, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
            sorted = tmp;
        }
        else
        {
            // N为奇数
            // even-pair
            if (rank % 2 == 0)
            {
                // [0, N - 2]做内部交换
                for (int i = 0; i < N - 1; i+=2)
                {
                    if (value[i] > value[i + 1])
                    {
                        swap(value + i, value + i + 1);
                        sorted = 0;
                    }
                }
                // 做边界上的通信
               if (rank != size - 1)
               {
                   // 如果size-1号进程为偶数进程 则无需通信
                    int* p_recv = (int*)malloc(sizeof(int));
                    // 向rank + 1发送尾部值
                    MPI_Request send_request;
                    MPI_Isend(value + N - 1, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send_request);
                    MPI_Recv(p_recv, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    if (value[N - 1] > *p_recv)
                    {
                        swap(value + N - 1, p_recv);
                        sorted = 0;
                    }
                    free(p_recv);
               }
            }
            else
            {
                // [1, N - 1]做内部交换
                for (int i = 1; i < N; i+=2)
                {
                    if (value[i] > value[i + 1])
                    {
                        swap(value + i, value + i + 1);
                        sorted = 0;
                    }
                }
                int* p_recv = (int*)malloc(sizeof(int));
                // 向rank - 1发送头部值
                MPI_Request send_request;
                MPI_Isend(value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &send_request);
                MPI_Recv(p_recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (*p_recv > value[0])
                {
                    swap(p_recv, value);
                    sorted = 0;
                }
                free(p_recv);
            }

            // odd-pair
            if (rank % 2 != 0)
            {
                // [0, N - 2]做内部交换
                for (int i = 0; i < N - 1; i+=2)
                {
                    if (value[i] > value[i + 1])
                    {
                        swap(value + i, value + i + 1);
                        sorted = 0;
                    }
                }
                // 做边界上的通信
               if (rank != size - 1)
               {
                   // 如果size-1号进程为偶数进程 则无需通信
                    int* p_recv = (int*)malloc(sizeof(int));
                    // 向rank + 1发送尾部值
                    MPI_Request send_request;
                    MPI_Isend(value + N - 1, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, &send_request);
                    MPI_Recv(p_recv, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                    if (value[N - 1] > *p_recv)
                    {
                        swap(value + N - 1, p_recv);
                        sorted = 0;
                    }
                    free(p_recv);
               }
            }
            else
            {
                // [1, N - 1]做内部交换
                for (int i = 1; i < N; i+=2)
                {
                    if (value[i] > value[i + 1])
                    {
                        swap(value + i, value + i + 1);
                        sorted = 0;
                    }
                }
                if (rank != 0)
                {
                    int* p_recv = (int*)malloc(sizeof(int));
                    // 向rank - 1发送头部值
                    MPI_Request send_request;
                    MPI_Isend(value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &send_request);
                    MPI_Recv(p_recv, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    if (*p_recv > value[0])
                    {
                        swap(p_recv, value);
                        sorted = 0;
                    }
                    free(p_recv);
                }
            }
            // 同步一下flag标志位
            int tmp = 0;
            MPI_Allreduce(&sorted, &tmp, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
            sorted = tmp;
        }
    }
    printInfo(rank, value);
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

void swap(int* p, int * q)
{
    int tmp = *p;
    *p = *q;
    *q = tmp;
}
