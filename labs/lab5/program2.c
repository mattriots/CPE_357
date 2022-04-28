#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

typedef unsigned char BYTE;

int main()
{
    ///////////////////////////////
    // CHECKING FOR CONNECTION   //
    ///////////////////////////////

    for (;;)
    {
        int fd = shm_open("sharedMem", O_RDWR, 0777);
        if (fd != -1)
        {
            break;
        }
    }

    // Print upon sucessful connection 

    printf("Successfully connected\n");


    //Make space for recieving input from user on THIS programs virtual mem

    BYTE *input = (BYTE *)mmap(NULL, 200, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Constanly check the input, waiting until there is something to print out

    for (;;)
    {
        if (input[0] != NULL)
        {
            printf("%s", input);
            break;
        }
    }

    ///////////////////////////////
    //  CLOSE AND CLEANUP       ///
    ///////////////////////////////

    close(fd);
    shm_unlink("sharedMem");
    munmap(input, 200);

    return 0;
}
