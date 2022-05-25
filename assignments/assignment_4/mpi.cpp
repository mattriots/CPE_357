#include <unistd.h>
#include <iostream>
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

int main(int argc, char *argv[])
{
    // char *args[3];
    // char *prog1name = argv[1];
    // char *numinstances = argv[2];
    // int numprog = atoi(numinstances);

    // int par_id = 1; // we should make a loop of how many instances and assign a par_id for each

    // args[0] = malloc(100);
    // args[1] = malloc(100);
    // args[2] = malloc(100);
    // args[3] = malloc(100);

    // strcpy(args[0], prog1name);
    // strcpy(args[1], par_id); /// assign this as we make more childrend and use execv
    // strcpy(args[2], numinstances);
    // strcpy(args[3], NULL);

    // for (int i = 0; i < numprog; i++)
    // {
    // if (fork() == 0)
    // {
    // args[0] = (char *)malloc(100);
    // args[1] = (char *)malloc(100);
    // args[2] = (char *)malloc(100);
    // args[3] = NULL;

    // // strcpy(args[0], prog1name);
    // args[0] = (char *)i;
    // args[1] = numinstances;
    // args[2] == NULL;
    // args[3] == NULL;

    // printf(args[0]);
    // printf(args[1]);
    // printf(args[2]);

    // execv(prog1name, args);

    // }
    // wait(0);

    char *args[4];

    args[0] = (char *)malloc(100);
    args[1] = (char *)malloc(100);
    args[2] = (char *)malloc(100);
    args[3] = NULL;

    // args[0] = new char[100];
    // args[1] = new char[100];
    // args[2] = new char[100];
    // args[3] = NULL;
    sprintf(args[0], argv[1]);
    sprintf(args[1], argv[2]);

    int n = atoi(argv[2]);

    char exe[100];
    char id[10];
    sprintf(exe, "./%s", args[0]);

    for (int i = 0; i < n; i++)
    {
        sprintf(args[2], "%d", i);
        if (fork() == 0)
        {
            execv(exe, args);
            cout << "couldnt do execv with" << exe << endl;
            // if successful it terminates the caller
            return 0;

            // execv("./calcmatrix", ("calcmatrix, "4", "0", "NULL"))
        }
    }
    wait(0);

    free(args[0]);
    free(args[1]);
    free(args[2]);
    // free (args[3]);
    return 0;
}