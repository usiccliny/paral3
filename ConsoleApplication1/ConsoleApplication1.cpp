#include <stdio.h>
#include <iostream>
#include "mpi.h"
using namespace std;

int main(int* argc, char** argv)
{
    MPI_Status status;
    int size, rank;
    int message = 0;
    int tag = 59;

    MPI_Init(argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double t1 = 0, t2 = 0;
    for (int i = 0; i < 100000; i++)
    {
        t1 += MPI_Wtime();
        if (rank == 0)
        {
            message = 123;
            MPI_Send(&message, 1, MPI_INT, 1, tag, MPI_COMM_WORLD);
        }
        else if (rank == 1)
        {
            MPI_Recv(&message, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        }

        t2 += MPI_Wtime();
    }
    if (rank == 1) cout << "SEND time: " << (t2 - t1)/100000 << endl;

    double t3 = 0, t4 = 0;
    for (int i = 0; i < 100000; i++)
    {
        t3 += MPI_Wtime();
        if (rank == 2)
        {
            int buffer_attached_size = MPI_BSEND_OVERHEAD + sizeof(int);
            char* buffer_attached = (char*)malloc(buffer_attached_size);
            MPI_Buffer_attach(buffer_attached, buffer_attached_size);

            message = 123;
            MPI_Bsend(&message, 1, MPI_INT, 3, tag, MPI_COMM_WORLD);

            MPI_Buffer_detach(&buffer_attached, &buffer_attached_size);
            free(buffer_attached);
        }
        else if (rank == 3)
        {
            MPI_Recv(&message, 1, MPI_INT, 2, tag, MPI_COMM_WORLD, &status);
        }

        t4 += MPI_Wtime();
    }

    if (rank == 3) cout << "BSEND time: " << (t4 - t3)/100000 << endl;

    double t5 = 0, t6 = 0;
    for (int i = 0; i < 100000; i++)
    {
        t5 += MPI_Wtime();
        MPI_Request request, request1;
        if (rank == 4)
        {
            message = 123;

            MPI_Isend(&message, 1, MPI_INT, 5, tag, MPI_COMM_WORLD, &request);
            MPI_Wait(&request, MPI_STATUS_IGNORE);
        }
        else if (rank == 5)
        {
            MPI_Irecv(&message, 1, MPI_INT, 4, tag, MPI_COMM_WORLD, &request1);
            MPI_Wait(&request1, MPI_STATUS_IGNORE);
        }

        t6 += MPI_Wtime();
    }

    

    if (rank == 5) cout << "ISEND time: " << (t6 - t5)/100000 << endl;


    MPI_Finalize();
}