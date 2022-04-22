
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

///////////////////////////////
// GLOBAL VARS AND HEADERS  ///
///////////////////////////////

int pi[2];

void signalfct(int i);

void changetext(char *text, int ra);

int main()
{
    ///////////////////////////////
    //   SETUP AND VARS         ///
    ///////////////////////////////

    // assign signalfct to be called by SIGUSR1
    signal(SIGUSR1, signalfct);

    // you can make 2 pipes to do 2 way communication
    pipe(pi);

    //Parent process id
    int parent_pid = getpid();

    //Save stdin behavior for later
    int save_stdin = dup(STDIN_FILENO);

    // Have to mmap some memory for the parent/child to share
    int *flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    ///////////////////////////////
    //  FORK AND CHILD          ///
    ///////////////////////////////

    if (fork() == 0)
    {
        for (;;)
        {
            close(pi[0]);
            *flag = 0; //Set flag to 0. So if after 10 seconds its still in active
                       //The if (flag == 0) is run

            for (int i = 1; i < 11; i++) //For loop to sleep for 10 seconds. 1 sec at a time
            {                            //This way we can jump out of sleep quickly
                sleep(1);
                // cout << i << endl;
                if (*flag == 1 || *flag == 2) // If something has been typed (1)
                    break;                    // Or 'q' has been entered (2). Break outta here
            }

            //If flag is 0 then there has been no activity
            if (*flag == 0)
            {
                kill(parent_pid, SIGUSR1); // Send the parent id the SIGURS1. Taking over the STDIN

                write(pi[1], "no activity detected\n", strlen("no activity detected\n")); //Write this to the terminal
                close(pi[1]);
            }

            //If flag is 2 then 'q' was entered and its time to break outta here
            if (*flag == 2)
            {
                break;
            }
        }

        return 0;
    }

    ///////////////////////////////
    //  PARENT PROCESS          ///
    ///////////////////////////////

    //make space for the incoming text
    char text[1000];
    close(pi[1]);

    for (;;) // infinite loop
    {
        dup2(save_stdin, STDIN_FILENO); //Restor STDIN for keyboard to work again

        int ra = read(STDIN_FILENO, &text[1], 100); // how many bytes we really read

        *flag = 1; //Set the flag to 1 cuz we are writing now!

        changetext(text, ra); //Change the text to add "!" to front and back

        if (text[1] == 'q') // just working for 'q' so far
        {
            *flag = 2; //set flag to 2 and get outta dodge
            break;
        }
    }

    close(pi[0]);

    wait(0);

    munmap(flag, sizeof(int)); //clean up space

    return 0;
}

///////////////////////////////
//   FUNCTIONS              ///
///////////////////////////////

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