#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <string>


// typedef unsigned char BYTE;

int fd;

int main()
{
    ///////////////////////////////
    // CHECKING FOR CONNECTION   //
    ///////////////////////////////

    for (;;)
    {
        fd = shm_open("sharedMem", O_RDWR, 0777); //0600
        if (fd != -1)
        {
            break;
        }
    }

    // Print upon sucessful connection 

    printf("Successfully connected\n");


    //Make space for recieving input from user on THIS programs virtual mem
    
     char *input = (char *)mmap(NULL, 200, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Constanly check the input, waiting until there is something to print out

    for (;;)
    {
        // if (input[0] != NULL)
        if(strlen(input) > 0)
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
