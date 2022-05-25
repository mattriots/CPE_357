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

using namespace std;
// message passing interface
//> ./mpi calcmatrix 4    <--calls calcmatrix 4 times

// calcmatrix 4 0
// calcmatrix 4 1
// calcmatrix 4 2
// calcmatrix 4 3

int main(int argc, char *argv[])
{

    char *args[4];

    args[0] = new char[100];
    args[1] = new char[100];
    args[2] = new char[100];
    args[3] = NULL;
    sprintf(args[0], argv[1]);
    sprintf(args[1], argv[2]);

    int n = atoi(argv[2]);

    char exe[100];
    char id[10];
    sprintf(exe, "./$s", argv[1]);

    for (int i = 0; i < n; i++)
    {
        sprintf(args[2], "%d", i);
        execv(exe, args);   //if successful it terminates the caller
    }

    // execv("./calcmatrix", args);
}