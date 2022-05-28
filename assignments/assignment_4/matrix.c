#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#define MATRIX_DIMENSION_XY 10

//************************************************************************************************************************
// sets one element of the matrix
void set_matrix_elem(float *M, int x, int y, float f)
{
    M[x + y * MATRIX_DIMENSION_XY] = f;
}
//************************************************************************************************************************
// lets see it both are the same
int quadratic_matrix_compare(float *A, float *B)
{
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            if (A[a + b * MATRIX_DIMENSION_XY] != B[a + b * MATRIX_DIMENSION_XY])
                return 0;

    return 1;
}
//************************************************************************************************************************
// print a matrix
void quadratic_matrix_print(float *C)
{
    printf("\n");
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
    {
        printf("\n");
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            printf("%.2f,", C[a + b * MATRIX_DIMENSION_XY]);
    }
    printf("\n");
}
//************************************************************************************************************************
// multiply two matrices
void quadratic_matrix_multiplication(float *A, float *B, float *C)
{
    // nullify the result matrix first
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            C[a + b * MATRIX_DIMENSION_XY] = 0.0;
    // multiply
    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)         // over all cols a
        for (int b = 0; b < MATRIX_DIMENSION_XY; b++)     // over all rows b
            for (int c = 0; c < MATRIX_DIMENSION_XY; c++) // over all rows/cols left
            {
                C[a + b * MATRIX_DIMENSION_XY] += A[c + b * MATRIX_DIMENSION_XY] * B[a + c * MATRIX_DIMENSION_XY];
            }
}
//************************************************************************************************************************

//************************************************************************************************************************
// multiply two matrices
// TODO: quadratic_matrix_multiplication_parallel(par_id, par_count,A,B,C, ...);
void quadratic_matrix_multiplication_parallel(int par_id, int par_count, float *A, float *B, float *C,  struct timeval *start)
{

    if (par_id == 0)
    {
        // nullify the result matrix first
        for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
            for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
                C[a + b * MATRIX_DIMENSION_XY] = 0.0;

        gettimeofday(start, NULL);
    }

    for (int a = 0; a < MATRIX_DIMENSION_XY; a++)                                                                               // over all cols a
        for (int b = ((MATRIX_DIMENSION_XY * par_id / par_count)); b < ((MATRIX_DIMENSION_XY * (par_id + 1) / par_count)); b++) // over rows
            for (int c = 0; c < MATRIX_DIMENSION_XY; c++)                                                                       // over all rows/cols left
            {
                C[a + b * MATRIX_DIMENSION_XY] += A[c + b * MATRIX_DIMENSION_XY] * B[a + c * MATRIX_DIMENSION_XY];
                // quadratic_matrix_print(C);
            }
}
//************************************************************************************************************************

// n = 4
// ready[0] = 0;
// ready[0] = 0;
// ready[0] = 0;
// ready[0] = 0;

void synch(int par_id, int par_count, int *ready, int ri) // ready[n]
{
    // TODO: synch algorithm. make sure, ALL processes get stuck here until all ARE here

    // Challenge
    ////No labels and no increments if you want 2% EC
    // Put video on piazza
    // for (int i = 0; i < 100; i++)

    ready[par_id]++;
    int leave = 1;
    while (leave)
    {
        leave = 0;
        for (int i = 0; i < par_count; i++)
        {
            if (ready[i] <= ri)
            {
                leave = 1;
            }
        }
    }
}
//************************************************************************************************************************
int main(int argc, char *argv[])
{

    int par_id = 0;   // the parallel ID of this process
    int par_count;    // the amount of processes
    float *A, *B, *C; // matrices A,B and C
    int *ready;       // needed for synch
    struct timeval start, end;

    if (argc != 3)
    {
        printf("no shared\n");
    }
    else
    {

        par_count = atoi(argv[1]);
        par_id = atoi(argv[2]);
    }
    if (par_count == 1)
    {
        printf("only one process\n");
    }

    // printf("par_count: %d\n", par_count);
    // printf("par_id: %d\n", par_id);

    int fd[4];
    if (par_id == 0)
    {
        // TODO: init the shared memory for A,B,C, ready. shm_open with C_CREAT here! then ftruncate! then mmap
        fd[0] = shm_open("matrixA", O_CREAT | O_RDWR, 0777);
        fd[1] = shm_open("matrixB", O_CREAT | O_RDWR, 0777);
        fd[2] = shm_open("matrixC", O_CREAT | O_RDWR, 0777);
        fd[3] = shm_open("synchobject", O_CREAT | O_RDWR, 0777);

        ftruncate(fd[0], MATRIX_DIMENSION_XY);
        ftruncate(fd[1], MATRIX_DIMENSION_XY);
        ftruncate(fd[2], MATRIX_DIMENSION_XY);
        ftruncate(fd[3], par_count * sizeof(int));

        A = (float *)mmap(NULL, MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        B = (float *)mmap(NULL, MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
        C = (float *)mmap(NULL, MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
        ready = (int *)mmap(NULL, par_count * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);

        // ready[n] = 0; // set all the ready[par_id] to 0
        for (int i = 0; i < par_count; i++)
        {
            ready[i] = 0;
        }
    }
    else
    {
        sleep(2);
        // TODO: init the shared memory for A,B,C, ready. shm_open withOUT C_CREAT here! NO ftruncate! but yes to mmap
        fd[0] = shm_open("matrixA", O_RDWR, 0777);
        fd[1] = shm_open("matrixB", O_RDWR, 0777);
        fd[2] = shm_open("matrixC", O_RDWR, 0777);
        fd[3] = shm_open("synchobject", O_RDWR, 0777);

        A = (float *)mmap(NULL, MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        B = (float *)mmap(NULL, MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
        C = (float *)mmap(NULL, MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
        ready = (int *)mmap(NULL, par_count * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);
    }

    // for (int i = 0; i < 100; i++){   //CHALLENGE

    // printf("par_id: %d at first synch \n", par_id);

    synch(par_id, par_count, ready, 0); // gather

    // printf("par_id: %d after first synch \n", par_id);

    // cout <<par_id << " " << i << endl;
    // }

    if (par_id == 0)
    {
        // TODO: initialize the matrices A and B
        // Need to make this a random number
        // srand(time(0));

        for (int a = 0; a < MATRIX_DIMENSION_XY; a++)
        {
            for (int b = 0; b < MATRIX_DIMENSION_XY; b++)
            {
                int num1 = (rand() % 9 - 1 + 1) + 1;
                int num2 = (rand() % 9 - 1 + 1) + 1;
                set_matrix_elem(A, a, b, num1);
                set_matrix_elem(B, a, b, num2);
            }
        }
    }

    // printf("par_id: %d at second synch \n", par_id);

    synch(par_id, par_count, ready, 1); // gather

    // printf("par_id: %d after second synch \n", par_id);

    if (par_count == 1)
    {
        quadratic_matrix_multiplication(A, B, C);
    }
    else
    {

        quadratic_matrix_multiplication_parallel(par_id, par_count, A, B, C, &start);
    }

    // printf("par_id: %d at third synch \n", par_id);

    synch(par_id, par_count, ready, 2); // gather

    // printf("par_id: %d after third synch \n", par_id);

    if (par_id == 0)
    {

        gettimeofday(&end, NULL); // end timing normal -- using time of day

        long duration = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

        printf("time taken: %d\n", duration);

        quadratic_matrix_print(C);
    }

    // printf("par_id: %d at fourth synch \n", par_id);

    synch(par_id, par_count, ready, 3); // gather

    // printf("par_id: %d after fourth synch \n", par_id);

    // lets test the result:
    float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
    quadratic_matrix_multiplication(A, B, M);
    if (quadratic_matrix_compare(C, M))
        printf("full points!\n");
    else
        printf("buuug!\n");

    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");
    munmap(A, MATRIX_DIMENSION_XY);
    munmap(B, MATRIX_DIMENSION_XY);
    munmap(C, MATRIX_DIMENSION_XY);
    munmap(ready, par_count * sizeof(int));
    return 0;
}