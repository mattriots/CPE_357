#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <dirent.h>
#include <fcntl.h>

using namespace std;


int main()
{
    // Can sleep here
    //  Or loop sleep(1) until fd != -1
    // Cuz fd will return -1 until it finds the shared
    // Memory name
    sleep(1);

    // Look for filedescriptor, remove the "O_CREAT"
    int fd = shm_open("ournamesharedmem", O_RDWR, 0777);

    // Make space again for this process
    // Because it has its own Virtual Memory
    int *p = (int *)mmap(NULL, 100 * sizeof(int), PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, 0);

    cout << p[0];

    close(fd);
    shm_unlink("ournamesharedmem");
    munmap(p, 100 * sizeof(int));
}