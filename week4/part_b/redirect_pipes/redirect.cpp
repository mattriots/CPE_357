#include <stdio.h>
#include <iostream>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>

using namespace std;

int main()
{

    int fd[2];
    pipe(fd);

    // int t = dup(fd[0]); //makes a duplicate fd
    // int realstdin = fileno(stdin); //another way to find fd of stdin

    char text[100]; /// buffer

    if (fork() == 0)
    {
        sleep(2);
        close(fd[0]);
        write(fd[1], "hello", 6);
        close(fd[1]);
        return 0;
    }
    else
    {

        close(fd[1]);

        int save_stdin = dup(STDIN_FILENO); // save stdin before overwritting

        dup2(fd[0], STDIN_FILENO); // will overwrite stdin
                                   // with what we send through "hello"
        close(fd[0]);
        scanf("%s", text);

        printf("%s\n", text);

        dup2(save_stdin, STDIN_FILENO);
        text[0] = 0;
        
        scanf("%s", text);
        // read(STDIN_FILENO, text, 20);
        // read(STDIN_FILENO, text, 20);
        printf("%s\n", text);

        wait(0);
    }

    return 0;
}