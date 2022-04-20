
#include <stdio.h>
#include <iostream>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

int pi[2];

void signalfct(int i)
{
    dup2(pi[0], STDIN_FILENO);
    cout << "signal" << endl;
}

int main()
{

    signal(SIGUSR1, signalfct);

    // you can make 2 pipes to do 2 way communication
    pipe(pi);

    // Have to mmap some memory for the parent/child to share

    int parent_pid = getpid();

    int save_stdin = dup(STDIN_FILENO); // duplicate stdin

    int *flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (fork() == 0) // child
    {
        /// wait for 10 seconds
        /// check if anything has been written
        // if not overwrite stdin and take control of keyboard
        // print out "inactivity detected"
        for (;;)
        {
            sleep(10);

            if (&flag == 1)
            {
                kill(parent_pid, SIGUSR1);

                write(pi[1], "I have enough\n", strlen("I have enough\n"));
                flag = 0;
            }
        }

        return 0;
    }

    char text[1000];
    for (;;) // infinite loop
    {
        dup2(save_stdin, STDIN_FILENO);
        int ra = read(STDIN_FILENO, text, 100); // how many bytes we really read
        text[ra] = 0;
        *flag = 1;
        cout << text << endl;
        if (text[0] == 'q')
            break;
    }

    wait(0);

    return 0;

    // //REDIRECTION

    // int y = dup(STDOUT_FILENO);  //y now acts like Std output
    //                             //save y for later

    // write(y, "hello everyone\n", 16); //printing to stdout via y

    // dup2(STDIN_FILENO,i[0]); //overwrites the "keyboard input"
    //                         //2nd arg into 1st

    // if (fork() == 0)
    // {
    //                                 //write returns how many bytes it actually writes
    //     write(i[1], "hello world", 12);
    //     return 0;
    // }

    // wait(0);
    // char text[1000];
    //                                 //read returns how many bytes it actually reads
    // cout << read(i[0], text, 100) << endl;
    // cout << text << endl;

    // write(1,"hello all",5); //write directly to stdin

    return 0;
}