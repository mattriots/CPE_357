#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

typedef unsigned char BYTE;

int main()
{

    int i = 0;

    //using pointer *p and MAP_SHARED the parent/child process will access the same point in memory
    //Useful when doing paraell bitmap editting. or matrx multiplication

    int *p = (int *)mmap(NULL, 4, PROT_READ | PROT_WRITE,
                           MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *p = 0;
    if (fork() == 0) // child process
    {
        cout << "I am the kid" << endl;

        *p = 6;

        return 0;
    }
    else // parent process
    {
        cout << "I am the parent" << endl;
    }
    
    wait(0);

    cout << "p = " << *p <<endl;

    munmap(p, 4);
    return 0;
}