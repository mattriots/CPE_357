#include <stdio.h>
#include <iostream>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

int main()
{
    int fd[2]; /// file descriptor fd[2]
    pipe(fd);  // A pipe is simply 2 files
                // 1 file you write to ---> fd[1] 
                // 1 file you read from --> fd[0]
                 //FD is in kernel
                 //To communicate 2 ways you need to pipes
                 //i.e, parent -> child  & child -> parent
        

    FILE *file = fopen("test.c", "r");

    FILE *g = fdopen(5,"r");    /// does the same thing as fopen

    char text[100];

    if (fork() == 0) // child
    {
        close(fd[0]); // Since we are only writing in child
                    // We dont need fd[0] so close it

        write(fd[1], "hello world", 30);               //write to fd[1]
        write(fd[1], "the world greets you back", 40);  //write(fd[1], size)

        close(fd[1]); //close for cleanup

        return 0;
    }
    else
    {
        close(fd[1]);// Since we are only reading in parent 
                    // We dont need fd[1] so close it
        read(fd[0], text, 30);    //read from fd[0]
        printf("%s\n", text);    
        
        read(fd[0], text, 40);  //read(fd[0], size)

        close(fd[0]);       //close for cleanup

        printf("%s\n", text);  //close for cleanup

        wait(0);
    }

    return 0;
}