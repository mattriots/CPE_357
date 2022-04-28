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

// Program 1 has to create the shared memory FIRST
//  Before any other program sharing the memory tries to
//  access it.

// program 1
int main()
{
    // Create shared name memory file descriptor // no size added yet
    int fd = shm_open("ournamesharedmem", O_RDWR | O_CREAT, 0777);

    // connects to a certain amount of mem - not allocated yet
    // ftruncate(file descriptor, size)
    ftruncate(fd, 100 * sizeof(int));

    // Make the space with mmap
    // Same as before EXCEPT we put fd in
    int *p = (int *)mmap(NULL, 100 * sizeof(int), PROT_READ | PROT_WRITE,
                         MAP_SHARED, fd, 0);

    // //CAN MAKE A SECOND SHARED MEMORY
    // int fd2 = shm_open("ournamesharedmem", O_RDWR | O_CREAT, 0777);
    // ftruncate(fd2, 100 * sizeof(int));
    // int *p = (int *)mmap(NULL, 100 * sizeof(int), PROT_READ | PROT_WRITE,
    //                      MAP_SHARED, fd2, 0);

    p[0] = 99;

    sleep(10);

    // cleanup
    close(fd);
    shm_unlink("ournamesharedmem");
    munmap(p, 100 * sizeof(int));
}

// // program 2
// int main()
// {
//     // Can sleep here
//     //  Or loop sleep(1) until fd != -1
//     // Cuz fd will return -1 until it finds the shared
//     // Memory name
//     sleep(1);

//     // Look for filedescriptor, remove the "O_CREAT"
//     int fd = shm_open("ournamesharedmem", O_RDWR, 0777);

//     // Make space again for this process
//     // Because it has its own Virtual Memory
//     int *p = (int *)mmap(NULL, 100 * sizeof(int), PROT_READ | PROT_WRITE,
//                          MAP_SHARED, fd, 0);

//     cout << p[0];

//     close(fd);
//     shm_unlink("ournamesharedmem");
//     munmap(p, 100 * sizeof(int));
// }