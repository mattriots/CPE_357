#include <stdio.h>
#include <string.h>
#include <iostream>

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

void mysignalhandler(int u) // Redirecting a signal
{
    cout << "attempted to interuppt \n";
}
void sigh2(int u) // Redirecting a signal
{
    cout << "USR1 signal invoked \n";
}

int main()
{

    // int pid = getpid();

    // cout << pid << endl;

    // // signal(SIGINT, mysignalhandler); //overrides/redirects CTRL+C with mysignalhandler
    // // signal(SIGTSTP, mysignalhandler); //overrides CTRL+Z with mysignalhandler
    // // signal(SIGQUIT, mysignalhandler); //overrides CTRL+ \ with mysignalhandler
    // //                                   //cannot override KILL signal

    // signal(SIGUSR1, mysignalhandler); // User defined ginal

    // kill(pid, SIGUSR1);

    // for (;;) // infinite loop
    // {

    //     cout << "hello \n";
    //     sleep(1); // sleep 1 sec
    // }

    ///////////////////////////////////////////

    int pid = getpid(); // parent pid ?

    cout <<"Parent pid: "<< pid << endl;

    // Create shared memory pointer for child/parent
    int *childs_pid = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    if (fork() == 0) // child
    {
        signal(SIGUSR1, sigh2); // Initialize override IN the child process

        *childs_pid = getpid(); // dereference pointer to assign value of child pid

        for (int i = 0;; i++) //infinite for loop - incrementing
        {
            printf("%d\n", i);
            sleep(1);
        }

        return 0;
    }
    // parent

    sleep(1); //sleep 1 second so *childs_pid in child process is able to getpid();

    int kidspid = *childs_pid;

    cout << "child's pid: " << kidspid << endl; // print out child pid
    
    sleep(4);
    
    kill(kidspid, SIGUSR1); //sends signal to pid
                            //kill(pid to send to, signal to send)

    sleep(1);

    kill(kidspid, SIGKILL);
    
    wait(0);

    return 0;
}