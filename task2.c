#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
    int rank, size, N, M, newN, choice;
    MPI_Init(&argc, &argv);
    setbuf(stdout, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int *A = NULL, *B = NULL, *C = NULL;
    int *smallA, *smallB, *smallC;

    if (rank == 0)
    {
        printf("N (rows): ");
        scanf("%d", &N);
        printf("M (cols): ");
        scanf("%d", &M);
        printf("Enter 1 to input values, 2 for random: ");
        scanf("%d", &choice);
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&choice, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int rem = N % size;
    newN = (rem == 0) ? N : N + (size - rem);
    int elementsInProcesses = (newN / size) * M;
    smallA = malloc(elementsInProcesses * sizeof(int));
    smallB = malloc(elementsInProcesses * sizeof(int));
    smallC = malloc(elementsInProcesses * sizeof(int));

    if (rank == 0)
    {
        A = malloc(newN * M * sizeof(int));
        B = malloc(newN * M * sizeof(int));
        C = malloc(newN * M * sizeof(int));
        if (choice == 1)
        {
            printf("Enter elements of A (%d elements):\n", N * M);
            for (int i = 0; i < N * M; i++)
                scanf("%d", &A[i]);
            printf("Enter elements of B (%d elements):\n", N * M);
            for (int i = 0; i < N * M; i++)
                scanf("%d", &B[i]);
        }
        else
        {
            srand(time(NULL));
            printf("Initial A :\n");
            for (int i = 0; i < N * M; i++)
            {
                A[i] = rand() % 100;
                printf("%d ", A[i]);
                if ((i + 1) % M == 0)
                printf("\n");
            }
            printf("Initial B :\n");
            for (int i = 0; i < N * M; i++)
            {
                B[i] = rand() % 100;
                printf("%d ", B[i]);
                if ((i + 1) % M == 0)
                    printf("\n");
            }
        }
        for (int i = N * M; i < newN * M; i++)
            A[i] = 0;
        for (int i = N * M; i < newN * M; i++)
            B[i] = 0;
    }

    MPI_Scatter(A, elementsInProcesses, MPI_INT, smallA, elementsInProcesses, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, elementsInProcesses, MPI_INT, smallB, elementsInProcesses, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < elementsInProcesses; i++)
        smallC[i] = smallA[i] + smallB[i];

    MPI_Gather(smallC, elementsInProcesses, MPI_INT, C, elementsInProcesses, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Final result (C = A + B):\n");
        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < M; j++)
            {
                printf("%d ", C[i * M + j]);
            }
            printf("\n");
        }
        free(A);
        free(B);
        free(C);
    }

    free(smallA);
    free(smallB);
    free(smallC);

    MPI_Finalize();
    return 0;
}
