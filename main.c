#include <mpi.h>
#include <stdio.h>

#define MSG_SLV 1

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    union {
        struct dane_t {
            int a;
            int b;
        } dane;
        char bufor[sizeof(struct dane_t)];
    } zm;

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == 0) {
        int i;
        int count;
        MPI_Status status;
        for (i = 1; i < size; i++) {
            MPI_Recv(&zm, sizeof(struct dane_t), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("Rank: %d, rank*2 %d\n", zm.dane.a, zm.dane.b);
            MPI_Get_count(&status, MPI_BYTE, &count);
            printf("Dostałem %d elementow od %d z tagiem %d\n",
                   count, status.MPI_SOURCE, status.MPI_TAG);
        }
    } else {
        zm.dane.a = rank;
        zm.dane.b = rank * 2;
        MPI_Send(&zm, sizeof(struct dane_t), MPI_BYTE, 0, MSG_SLV, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
