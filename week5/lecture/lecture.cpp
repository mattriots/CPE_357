#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// program 1
int main()
{

    int fd = shm_open("supermario", O_CREAT | O_RDWR, 0777);

    char *pi = (char*)mmap(NULL, 100, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    pi = NULL;
}

// Linking program

int main()
{

    int fd = shm_open("supermario", O_RDWR, 0777);
}


// 1. start p1 and create nsm
// 2. start p2 and got into loop til something is in the mem
// 3. keyboard to nsm in P1
// 4. P2 prints it and terminates
// 5. quit P1



//To make chat program...
//mutex
//gather /synch

//-lrt