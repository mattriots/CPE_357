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
void showstat(char *filepath);
void findfile(char *filetofind, char *startdir, char *result, int search_in_all_subdirs);
void change(char *input);
void signalfct(int i);
void parseArgs(char *args, char **arg_tokens);
void pidarr(int *child_pids, int pid, int op, int childcount);

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
    char args[BUFFERSIZE];
    char *arg_tokens[10] = {0};
    // int child_pids[10] = {0};
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
    // text[0] = NULL;

    int status = 0;

    char findtext[30];
    char text[100];
    childcount = 0;
    *keyboardmode = 1;

    ///////////////////////////////
    //  PARENT PROCESS          ///
    ///////////////////////////////

    // close(pi[1]);

    for (;;) // infinite loop
    {

        // sleep(2);

        write(STDIN_FILENO, "findstuff$ ", 11);

        int ra = read(STDIN_FILENO, args, BUFFERSIZE); // how many bytes we really read

        args[ra] = '\0';

        printf("%s\n", args);

        dup2(save_stdin, STDIN_FILENO); // Restor STDIN for keyboard to work again

        *keyboardmode = 1;

        args[strcspn(args, "\n")] = 0; // finds the  new line character that is getting added onto the end of fgets and puts a 0 there.
                                       // Thus cleaning up the args array after each input. nice.

        parseArgs(args, arg_tokens);
        // Using mmap variables to child can read too
        firstarg = arg_tokens[0];
        filetofind = arg_tokens[1];
        flagchar = arg_tokens[2];

        /// Printing for now - to read
        // printf("0: %s\n", arg_tokens[0]);
        // printf("1: %s\n", arg_tokens[1]);
        // printf("2: %s\n", arg_tokens[2]);

        if (strcmp(firstarg, "quit") == 0)
        {
            kill(*childs_pid, SIGKILL);
            waitpid(*childs_pid, &status, WNOHANG);
            return 0;
        }

        ///////////////////////////////
        //  FORK AND CHILD          ///
        ///////////////////////////////

        /// NEED TO FIGURE OUT HOW TO HANDLE CHILDREN THE BEST!
        // ARRAY!? or just count!?  But the numbers always change within the children process
        // Ponder this

        if (strcmp(firstarg, "find") == 0 || strcmp(firstarg, "quit") == 0)
        {

            // printf("childcount outside: ");
            // printf("%d\n", childcount);
            *keyboardmode = 0;

            if (fork() == 0)
            {

                sleep(10);
                close(pi[0]);
                *childs_pid = getpid();
                // childcount++;

                // printf("childcount begin: ");
                // printf("%d\n", childcount);
                // pidarr(child_pids, *childs_pid, 1, *childcount);

                // cout << "im inside a child" << endl;

                // // kidspid = *childs_pid;

                if (flagchar == NULL || flagchar == "") // No flag we will just seach in the current directory
                {

                    findfile(filetofind, workdir, result, 0);
                    kill(parent_pid, SIGUSR1);
                    write(pi[1], strcat(result, "\0"), strlen(strcat(result, "\0")));
                }

                else if (flagchar[0] == '-' && //-s flag -> search in current and all sub dirs
                         flagchar[1] == 's')
                {

                    findfile(filetofind, workdir, result, 1);
                    kill(parent_pid, SIGUSR1);
                    write(pi[1], strcat(result, "\0"), strlen(strcat(result, "\0")));
                }
                else if (flagchar[0] == '-' && //-f -> search in all dires
                         flagchar[1] == 'f')
                {

                    findfile(filetofind, alldir, result, 1);
                    kill(parent_pid, SIGUSR1);
                    write(pi[1], strcat(result, "\0"), strlen(strcat(result, "\0")));
                }

                else
                { // NEEED TO FIX THIS....Put it into write? Make it simpler...//
                    printf("not a valid flag\n");
                    printf("Usage: find [filename] [flag]\n");
                    printf("Flags:\n");
                    printf("-s: search in current dir and all sub-dirs\n");
                    printf("-f: search in all dirs starting at '/'\n");
                    printf("no flag: search in current dir only\n\n");
                }

                if (result[0] == NULL)
                {
                    write(pi[1], "> no file found < \n\0", strlen("> no file found < \n\0"));
                }

                close(pi[1]);

                result[0] = NULL; // Zero out result so there's a clean pipe everytime ?
                // childcount--;
                // printf("childcount end: ");
                // printf("%d\n", childcount);
                // pidarr(child_pids, *childs_pid, 0, *childcount);
                return 0;
            }

            // zero out the array so theres no left overs between searches
            for (int i = 0; i < 10; i++)
                arg_tokens[i] = 0;
        }

        waitpid(*childs_pid, &status, WNOHANG);
    }

    // munmap(flag, sizeof(int)); // clean up space
    munmap(childs_pid, sizeof(int));
    munmap(flagchar, 10); // clean up space
    munmap(filetofind, 100);
    munmap(firstarg, 100);
    munmap(childcount, sizeof(int));

    close(pi[0]);

    return 0;
}

void pidarr(int *child_pids, int pid, int op, int childcount)
{

    if (childcount == 10)
    {
        printf("you have too many children. pls wait");
        return;
    }
    /// Remove pid
    if (op == 0)
    {
        printf("removing a child");
        for (int i = 0; i < 10; i++)
        {
            printf("%d\n", child_pids[i]);
            if (child_pids[i] == pid)
            {
                child_pids[i] = 0;
                childcount--;
                printf("childcount: ");
                printf("%d\n", childcount);
            }
        }
    }

    // Add pid
    else
    {
        for (int i = 0; i < 10; i++)
        {
            printf("adding a child");
            if (child_pids[i] == 0)
            {
                printf("%d\n", child_pids[i]);
                child_pids[i] = pid;
                childcount++;
                printf("childcount: ");
                printf("%d\n", childcount);
                break;
            }
        }
    }
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
        // cout << "" << endl;
        return;
    }

    for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
    {

        char *name = entry->d_name;

        if (strcmp(name, filetofind) == 0)
        {
            // strcpy(result, startdir);
            strcat(result, filetofind);
            strcat(result, "\t");
            strcat(result, startdir);
            strcat(result, "\n");

            // kill(getppid(), SIGUSR1);
            // write(pi[1], strcat(startdir, "\n\0"), strlen(strcat(startdir, "\n\0")));
            break;
        }

        // directory = 4
        // search all dirs = 1 means seach all dirs
        // check to make sure we dont look into any of the '.' or '..' folders
        // But do i need to account for hidden folders with "," before their name?!?
        if (entry->d_type == 4 && search_in_all_subdirs == 1 && entry->d_name[0] != '.')
        {
            char nextdir[1000];
            stpcpy(nextdir, startdir);
            strcat(nextdir, "/");
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

    // printf("is this working\n");
}

// Show stat in here in case we wanna look at stats of files
// Prob just need the file type and name?
void showstat(char *filepath)
{

    struct stat sb; // Struct for stat. Containing all the necessary
                    // variables to store the file info

    int ret = stat(filepath, &sb);
    // stat(file name, address of structure ^)
    // Returns 0 on success, -1 on error

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT)
    {
    case S_IFBLK:
        printf("block device\n");
        break;
    case S_IFCHR:
        printf("character device\n");
        break;
    case S_IFDIR:
        printf("directory\n");
        break;
    case S_IFIFO:
        printf("FIFO/pipe\n");
        break;
    case S_IFLNK:
        printf("symlink\n");
        break;
    case S_IFREG:
        printf("regular file\n");
        break;
    case S_IFSOCK:
        printf("socket\n");
        break;
    default:
        printf("unknown?\n");
        break;
    }

    printf("I-node number:            %ju\n", (uintmax_t)sb.st_ino);

    printf("Mode:                     %jo (octal)\n",
           (uintmax_t)sb.st_mode);

    printf("Link count:               %ju\n", (uintmax_t)sb.st_nlink);
    printf("Ownership:                UID=%ju   GID=%ju\n",
           (uintmax_t)sb.st_uid, (uintmax_t)sb.st_gid);

    printf("Preferred I/O block size: %jd bytes\n",
           (intmax_t)sb.st_blksize);
    printf("File size:                %jd bytes\n",
           (intmax_t)sb.st_size);
    printf("Blocks allocated:         %jd\n",
           (intmax_t)sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));
}