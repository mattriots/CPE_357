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

int main(int argc, char *argv[])
{

    char *args[4];

    args[0] = (char *)malloc(100);
    args[1] = (char *)malloc(100);
    args[2] = (char *)malloc(100);
    args[3] = NULL;


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
            printf("couldn't do exec with %s", exe);
            // if successful it terminates the caller
            return 0;
        }
    }
    wait(0);

    free(args[0]);
    free(args[1]);
    free(args[2]);
    return 0;
}