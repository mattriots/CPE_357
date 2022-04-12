#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

using namespace std;

typedef unsigned char BYTE;

int main()
{

    int i = 0;
    int pid;

    cout << "PID: " << getpid() << endl; //Program ID

    // using pointer *p and MAP_SHARED the parent/child process will access the same point in memory

    int *p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE,
                         MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (fork() == 0) // child process
    {
        cout << "I am the kid" << endl;
       
        cout << "PID Child: " << getpid() << endl;

        *p = getpid();

        return 2; //Can change to number other than 1. Will return that number bit shifted -> by 8
    }
    else // parent process
    {
        cout << "I am the parent" << endl;
    }

    int status;
    // wait(0); //Wait for all my children
    waitpid(*p ,&status, 0); //wait for PID

    cout << "status = " << status << endl;

    munmap(p, 4);
    return 0;
}