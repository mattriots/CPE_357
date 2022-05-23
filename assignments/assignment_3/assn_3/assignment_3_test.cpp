#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <cstring>

#define BUFFERSIZE 1000

using namespace std;

int pi[2];
int status;
void findfile(char *filetofind, char *startdir, char *result, int search_in_all_subdirs);
void signalfct(int i);
void parseArgs(char *args, char **arg_tokens);

int main()
{
    ///////////////////////////////
    //   SETUP AND VARS         ///
    ///////////////////////////////

    signal(SIGUSR1, signalfct);

    pipe(pi);

    // Parent process id
    int parent_pid = getpid();

    // Save stdin behavior for later
    int save_stdin = dup(STDIN_FILENO);

    char workdir[BUFFERSIZE];
    char result[BUFFERSIZE];
    char args[10000];
    char *arg_tokens[10] = {0};
    result[0] = NULL; // to check if it's been changed later

    char *alldir = "/";

    getcwd(workdir, 1000);

    // Have to mmap some memory for the parent/child to share
    char *firstarg = (char *)mmap(NULL, 100, PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char *filetofind = (char *)mmap(NULL, 100, PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char *flagchar = (char *)mmap(NULL, 10, PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *childs_pid = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *childcount = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *child_pids = (int *)mmap(NULL, 10 * sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *keyboardmode = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *keyboardmode = 1;

    for (int i = 0; i < 10; i++)
    {
        child_pids[i] = -1;
    }

    ///////////////////////////////
    //  PARENT PROCESS          ///
    ///////////////////////////////

    // close(pi[1]);

    for (;;) // infinite loop
    {

        write(STDIN_FILENO, "findstuff$ ", 11);

        int ra = read(STDIN_FILENO, args, 10000); // how many bytes we really read

        args[ra - 1] = '\0';

        if (*keyboardmode == 0)
        {
            printf("%s\n", args);
            *keyboardmode = 1;
        }

        dup2(save_stdin, STDIN_FILENO); // Restor STDIN for keyboard to work again

        parseArgs(args, arg_tokens);

        // Using mmap variables to child can read too
        firstarg = arg_tokens[0];
        filetofind = arg_tokens[1];
        flagchar = arg_tokens[2];

        if (strcmp(firstarg, "quit") == 0)
        {
            kill(*childs_pid, SIGKILL);
            waitpid(*childs_pid, &status, 0);
            return 0;
        }

        ///////////////////////////////
        //  FORK AND CHILD          ///
        ///////////////////////////////
        if (*childcount >= 9)
        {
            fprintf(stderr, "too many child processes. please wait...");
        }
        if (strcmp(firstarg, "find") == 0 && *childcount < 9)
        {

            if (fork() == 0)
            {

                *childs_pid = getpid();
                (*childcount)++;

                close(pi[0]);

                if (flagchar == NULL || flagchar == "") // No flag we will just search in the current directory
                {

                    findfile(filetofind, workdir, result, 0);
                    kill(parent_pid, SIGUSR1);
                    *keyboardmode = 0;
                    write(pi[1], strcat(result, "\0"), strlen(strcat(result, "\0")));
                }

                else if (flagchar[0] == '-' && //-s flag -> search in current and all sub dirs
                         flagchar[1] == 's')
                {

                    findfile(filetofind, workdir, result, 1);
                    kill(parent_pid, SIGUSR1);
                    *keyboardmode = 0;
                    write(pi[1], strcat(result, "\0"), strlen(strcat(result, "\0")));
                }
                else if (flagchar[0] == '-' && //-f -> search in all dires
                         flagchar[1] == 'f')
                {

                    findfile(filetofind, alldir, result, 1);
                    kill(parent_pid, SIGUSR1);
                    *keyboardmode = 0;
                    write(pi[1], strcat(result, "\0"), strlen(strcat(result, "\0")));
                }

                if (result[0] == NULL)
                {
                    kill(parent_pid, SIGUSR1);
                    *keyboardmode = 0;
                    write(pi[1], "\n> no file found < \n\0", strlen("\n> no file found < \n\0"));
                }

                close(pi[1]);

                result[0] = NULL; // Zero out result so there's a clean pipe everytime 

                (*childcount)--;

                return 0;
            }

            waitpid(*childs_pid, &status, WNOHANG);


            // zero out the array so theres no left overs between searches
            for (int i = 0; i < 10; i++)
                arg_tokens[i] = 0;
        }

    }

    munmap(firstarg, 100);
    munmap(filetofind, 100);
    munmap(flagchar, 10); // clean up space
    munmap(childs_pid, sizeof(int));
    munmap(childcount, sizeof(int));
    munmap(child_pids, 10 * sizeof(int));
    munmap(keyboardmode, sizeof(int));

    close(pi[0]);

    return 0;
}

void parseArgs(char *args, char **arg_tokens)
{
    int num_args = 0;                // number of args entered
    char *token = strtok(args, " "); // String tokenizer. Arg and space delimeter

    while (token != NULL)
    {
        arg_tokens[num_args] = token;
        token = strtok(NULL, " ");
        num_args++;
    }
}

void findfile(char *filetofind, char *startdir, char *result, int search_in_all_subdirs)
{
    DIR *dir;
    struct dirent *entry;
    char workdir[1000];

    dir = opendir(startdir);

    if (dir == NULL)
    {
        return;
    }

    for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
    {

        char *name = entry->d_name;

        if (strcmp(name, filetofind) == 0)
        {
            // strcpy(result, startdir);
            result[0] = '\n';
            strcat(result, filetofind);
            strcat(result, "\t");
            strcat(result, startdir);
            strcat(result, "\n");
            break;
        }

        // directory = 4
        // search all dirs = 1 means seach all dirs
        // check to make sure we dont look into any of the '.' or '..' folders
        if (entry->d_type == 4 && search_in_all_subdirs == 1 && entry->d_name[0] != '.')
        {
            char nextdir[1000];
            stpcpy(nextdir, startdir);
            if (strcmp(nextdir, "/") != 0)
            {
                strcat(nextdir, "/");
            }
            strcat(nextdir, entry->d_name);

            findfile(filetofind, nextdir, result, search_in_all_subdirs);
        }
    }

    closedir(dir);
    return;
}

void signalfct(int i)
{
    dup2(pi[0], STDIN_FILENO);
}