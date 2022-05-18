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
void parse(char *text);

// int pi[2];

int main()
{
    ///////////////////////////////
    //   SETUP AND VARS         ///
    ///////////////////////////////

    char workdir[1000];
    char result[1000];
    // to check if it's been changed later

    getcwd(workdir, 1000);

    // assign signalfct to be called by SIGUSR1
    signal(SIGUSR1, signalfct);

    // Parent process id
    int parent_pid = getpid();

    // Save stdin behavior for later
    int save_stdin = dup(STDIN_FILENO);

    // Have to mmap some memory for the parent/child to share
    int *flag = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                            MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    char *text = (char *)mmap(NULL, 1000, PROT_READ | PROT_WRITE,
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    int *childs_pid = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    text[0] = NULL;

    int kidspid;
    int status = 0;

    ///////////////////////////////
    //  FORK AND CHILD          ///
    ///////////////////////////////

    // if (fork() == 0)
    // {
    //     for (;;)
    //     {

    //         *flag = 0; // Set flag to 0. So if after 10 seconds its still inactive
    //                    // The if (flag == 0) is run

    //         // If flag is 0 then there has been no activity
    //         if (text[0] != NULL)
    //         {
    //             // kill(parent_pid, SIGUSR1); // Send the parent id the SIGURS1. Taking over the STDIN
    //             printf("%s\n", "in child");

    //             findfile(text, workdir, result, 1);

    //             if (result[0] == NULL)
    //             {
    //                 cout << ">file not found<" << endl;
    //             }
    //             else
    //             {
    //                 cout << text << " " << result << endl;
    //             }
    //             break;
    //         }

    //         // If flag is 2 then 'q' was entered and its time to break outta here
    //         if (*flag == 2)
    //         {
    //             break;
    //         }
    //     }

    //     return 0;
    // }

    ///////////////////////////////
    //  PARENT PROCESS          ///
    ///////////////////////////////

    for (;;) // infinite loop
    {
        // dup2(save_stdin, STDIN_FILENO); // Restor STDIN for keyboard to work again

        // int ra = read(STDIN_FILENO, &text[1], 100); // how many bytes we really read
        printf("findstuff$ ");
        scanf("%s", text);

        if (text[0] == 'q' && // *** type 'quit' to quit ***
            text[1] == 'u' &&
            text[2] == 'i' &&
            text[3] == 't')
        {
            *flag = 2; // set flag to 2 and get outta dodge
            break;
        }

        parse(&text[0]);

        // *flag = 1; // Set the flag to 1 cuz we are writing now!

        result[0] = NULL;

        findfile(text, workdir, result, 1);

        if (result[0] == NULL)
        {
            printf(">file not found<\n");
        }
        // else
        // {
        //     cout << text << " " << result << endl;
        // }
        // break;

        // *childs_pid = fork();
        // kidspid = *childs_pid;
        // cout << "child's pid: " << kidspid << endl; // print out child pid
        // cout << "text" << text << endl;
    }

    waitpid(kidspid, &status, 0);

    kill(kidspid, SIGKILL);

    munmap(flag, sizeof(int)); // clean up space
    munmap(text, 1000);
    munmap(childs_pid, sizeof(int));

    return 0;
}

void parse(char *text)
{

    if (text[0] != 'f' && // *** type 'quit' to quit ***
        text[1] != 'i' &&
        text[2] != 'n' &&
        text[3] != 'd')
    {
        printf("invalid input. try again");
    }
       if (text != "find")
    {
        printf("invalid input. try again");
    }

    if(text[])
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
        // cout << name << endl;
        // showstat(entry->d_name);

        if (strcmp(name, filetofind) == 0)
        {
            strcpy(result, startdir);
            // cout << "found it" << endl;
            cout << filetofind << " " << startdir << endl;
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