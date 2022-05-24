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
    char *prog1name = argv[1];
    char *numinstances = argv[2];
    int numprog = atoi(numinstances);

    // int par_id = 1; // we should make a loop of how many instances and assign a par_id for each

    // args[0] = malloc(100);
    // args[1] = malloc(100);
    // args[2] = malloc(100);
    // args[3] = malloc(100);

    // strcpy(args[0], prog1name);
    // strcpy(args[1], par_id); /// assign this as we make more childrend and use execv
    // strcpy(args[2], numinstances);
    // strcpy(args[3], NULL);

    for (int i = 0; i < numprog; i++)
    {
        if (fork() == 0)
        {
            args[0] = (char*)malloc(100);
            args[1] = (char*)malloc(100);
            args[2] = (char*)malloc(100);
            // args[3] = malloc(100);

            // strcpy(args[0], prog1name);
            args[0] = (char*)i;
            args[1] = numinstances;
            args[2] == NULL;

            printf(args[0]);
            printf(args[1]);
            printf(args[2]);

            execv(prog1name, args);

        }
        wait(0);
    }
    return 0;
}