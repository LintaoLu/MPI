/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil ; -*- */
/*
 * See COPYRIGHT in top-level directory.
 */

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

/*
 * Sort array using blocking send/recv between 2 ranks.
 *
 * The master process prepares the data and sends the latter half
 * of the array to the other rank. Each rank sorts it half. The
 * master then merges the sorted halves together. The two ranks
 * communicate using blocking send/recv.
 */

#define NUM_ELEMENTS 50

int block_size;

static int compare_int(const void *a, const void *b)
{
    return (*(int *) a - *(int *) b);
}

/* Merge sorted arrays a[] and b[] into a[].
 * Length of a[] must be sum of lengths of a[] and b[] */
static void merge(int *a)
{  
    /* display sorted array */
        printf("Partially Sorted:\t\t");
        for (int i = 0; i < NUM_ELEMENTS; i++)
            printf("%d ", a[i]);
        printf("\n");    
    qsort(a, NUM_ELEMENTS, sizeof(int), compare_int);
}

int main(int argc, char **argv)
{
    int rank, size, data[NUM_ELEMENTS];
    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    block_size = NUM_ELEMENTS/size;

    srand(0);

    if (rank == 0) {
        /* prepare data and display it */
        int i;
        printf("Unsorted:\t");
        for (i = 0; i < NUM_ELEMENTS; i++) {
            data[i] = rand() % NUM_ELEMENTS;
            printf("%d ", data[i]);
        }
        printf("\n");
        
        int index;
        for(index = 1; index < size; index++){
            /* send block_size of the data to the other rank */
            MPI_Send(&data[index*block_size], block_size, MPI_INT, index, 0, MPI_COMM_WORLD); 
        }

        /* sort the first half of the data */
        qsort(data, block_size, sizeof(int), compare_int);
      
        for(index = 1; index < size; index++){
            /* receive sorted second half of the data */
            MPI_Recv(&data[index*block_size], block_size, MPI_INT, index, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        /* merge the two sorted halves (using sort on the whole array) */
        merge(data);

        /* display sorted array */
        printf("Sorted:\t\t");
        for (i = 0; i < NUM_ELEMENTS; i++)
            printf("%d ", data[i]);
        printf("\n");
    }
    else{
        /* receive block_size of the data */
        MPI_Recv(data, block_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        /* sort the received data */
        qsort(data, block_size, sizeof(int), compare_int);
        /* send back the sorted data */
        MPI_Send(data, block_size, MPI_INT, 0, 0, MPI_COMM_WORLD);  
    }

    MPI_Finalize();
    return 0;
}
