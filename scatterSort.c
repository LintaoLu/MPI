#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define NUM_ELEMENTS 50

int block_size, num_of_blocks;

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
    num_of_blocks = size;
    int *sub_nums = (int*) malloc(sizeof(int*)*block_size);

    srand(0);

    if (rank == 0) {
        /* prepare data and display it */
        printf("Unsorted:\t");
        for (int i = 0; i < NUM_ELEMENTS; i++) {
            data[i] = rand() % NUM_ELEMENTS;
            printf("%d ", data[i]);
        }
        printf("\n");
    }

    MPI_Scatter(&data, block_size, MPI_INT, sub_nums, block_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    qsort(sub_nums, block_size, sizeof(int), compare_int);

    MPI_Gather(sub_nums, block_size, MPI_INT, &data, block_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        /* merge the two sorted halves (using sort on the whole array) */
        merge(data);

        /* display sorted array */
        printf("Sorted:\t\t");
        for (int i = 0; i < NUM_ELEMENTS; i++)
            printf("%d ", data[i]);
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}
