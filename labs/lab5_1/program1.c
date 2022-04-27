#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// int fd;
// int fd[1];

typedef unsigned char BYTE;

int main()
{

    ///////////////////////////////
    // INITIALIZE AND MAKE SPACE //
    ///////////////////////////////

    //Create a Named Shared Memory and set that to fd
    int fd = shm_open("sharedMem", O_CREAT | O_RDWR, 0600);

    //Allot the space
    ftruncate(fd, 100);

    //Byte pointer to actually take in the input
    BYTE *input = (BYTE *)mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    ///////////////////////////////
    //  READ INPUT              ///
    ///////////////////////////////

    //Waiting for input from user on STDIN
    //IF user enters "quit" then break out
    for (;;)
    {
        read(STDIN_FILENO, input, 100);

        if (input[0] == 'q' && input[1] == 'u' && input[2] == 'i' && input[3] == 't')
        {
            break;
        }
    }

    ///////////////////////////////
    //  CLOSE AND CLEANUP       ///
    ///////////////////////////////
    close(fd);
    shm_unlink("sharedMem");
    munmap(input, 100);

    return 0;
}
