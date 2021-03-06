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

using namespace std;

void showstat(char *filepath);
void findfile(char *filetofind, char *startdir, char *result, int search_in_all_subdirs);
void change(char *input);
void signalfct(int i);

// int pi[2];

int main()
{

    ///////////////////////////////
    //   SETUP AND VARS         ///
    ///////////////////////////////
    char workdir[1000];
    char result[1000];
    result[0] = NULL; // to check if it's been changed later

    int status;

    char *filename = "findme.txt";
    char *alldir = "/";

    getcwd(workdir, 1000);

    // assign signalfct to be called by SIGUSR1
    signal(SIGUSR1, signalfct);

    // you can make 2 pipes to do 2 way communication
    // Just one for now
    // pipe(pi);

    // Parent process id
    int parent_pid = getpid();

    // Save stdin behavior for later
    int save_stdin = dup(STDIN_FILENO);

    // Have to mmap some memory for the parent/child to share
    int *flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *childs_pid = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char *text = (char *)mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    ///////////////////////////////
    //  FORK AND CHILD          ///
    ///////////////////////////////

    for (int i = 0; i < 3; i++)
    {
        if (fork() == 0)
        {
            // for (;;)
            // {
            // close(pi[0]);
            // *flag = 0; // Set flag to 0. So if after 10 seconds its still inactive
            //            // The if (flag == 0) is run

            // // If flag is 0 then there has been no activity
            // if (*flag == 3)
            // {
            //     // kill(parent_pid, SIGUSR1); // Send the parent id the SIGURS1. Taking over the STDIN

            //     // write(pi[1], "no activity detected\n", strlen("no activity detected\n")); // Write this to the terminal
            //     printf(">nothing found<");
            // }

            // *childs_pid = getpid();

            if (text[0] != NULL)
            {
                printf("in child\n");
                printf("%s\n", text);
                // break;
            }

            findfile(text, workdir, result, 1);

            // if (result[0] == NULL)
            // {
            //     cout << ">file not found<" << endl;
            // }
            // else
            // {
            //     cout << text << " " << result << endl;
            // }

            // printf(result);

            // if (*flag == 2)
            // {
            //     // break;
            // }
            // }

            // close(pi[1]);
            // return 0;
        }

        ///////////////////////////////
        //  PARENT PROCESS          ///
        ///////////////////////////////

        // close(pi[1]);

        sleep(1);

        int kidspid;

        // for (int i = 0; i < 3; i++) // infinite loop
        // {
        // dup2(save_stdin, STDIN_FILENO); // Restor STDIN for keyboard to work again

        // int ra = read(STDIN_FILENO, text, 100); // how many bytes we really read
        scanf("%s", text);

        if (text[0] == 'q' && // *** type 'quit' to quit ***
            text[1] == 'u' &&
            text[2] == 'i' &&
            text[3] == 't')
        {
            *flag = 2; // set flag to 2 and get outta dodge
            break;
        }

        *childs_pid = fork();

        kidspid = *childs_pid;

        cout << "child's pid: " << kidspid << endl; // print out child pid

        // *flag = 1; // Set the flag to 1 cuz we are writing now!
        // }

        waitpid(kidspid, &status, 0);

        kill(kidspid, SIGKILL);
    }

    // close(pi[0]);

    munmap(flag, sizeof(int)); // clean up space
    munmap(childs_pid, sizeof(int));
    munmap(text, 1000);

    return 0;
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
        // cout <<"startdir: " << startdir << endl;

        char *name = entry->d_name;
        cout << name << endl;
        // showstat(entry->d_name);

        if (strcmp(name, filetofind) == 0)
        {
            strcpy(result, startdir);
            cout << "found it" << endl;
            cout << startdir << endl;
            return;
        }

        // directory = 4
        // search all dirs = 1 means seach all dirs
        // check to make sure we dont look into any of the '.' or '..' folders
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
    // dup2(pi[0], STDIN_FILENO);
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