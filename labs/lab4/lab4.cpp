
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
}

void changetext(char *text, int ra)
{
    text[0] = '!';
    text[ra] = '!';
    text[ra + 1] = 0;
    cout << text << endl;
}

int main()
{
    // assign signalfct to be called by SIGUSR1
    signal(SIGUSR1, signalfct);

    // you can make 2 pipes to do 2 way communication
    pipe(pi);

    int parent_pid = getpid();

    int save_stdin = dup(STDIN_FILENO); // duplicate stdin

    // Have to mmap some memory for the parent/child to share
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
            *flag = 0;
            // sleep(10);
            for (int i = 1; i < 11; i++)
            {
                sleep(1);
                // cout << i << endl;
                if (*flag == 1 || *flag == 2)
                    break;
            }

            if (*flag == 0)
            {
                kill(parent_pid, SIGUSR1);

                write(pi[1], "no activity detected\n", strlen("no activity detected\n"));
            }
            if(*flag == 2)
            {
                break;
            }
        }
        return 0;
    }

    
    char *text = (char *)malloc(1000); //make space for the incoming text

    for (;;) // infinite loop
    {
        dup2(save_stdin, STDIN_FILENO);

        int ra = read(STDIN_FILENO, &text[1], 100); // how many bytes we really read

        *flag = 1;

        changetext(text, ra);

        if (text[1] == 'q') // just working for 'q' so far
        {
            *flag = 2;
            break;
        }
            
    }

    wait(0);

    munmap(flag, sizeof(int));
    free(text);

    return 0;
}