#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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

int main()
{
    char workdir[1000];
    char result[1000];
    char test[1000]; 

    getcwd(workdir, 1000);

    findfile("findme.txt", workdir, result, 1);
    change(test);

    char *ween = result;

    cout << "findme.txt " << result << endl;
    cout << "test " << test << endl;

    return 0;
}

void change(char *input)
{
    input = "suck it";
}

void findfile(char *filetofind, char *startdir, char result[], int search_in_all_subdirs)
{
    DIR *dir;
    struct dirent *entry;
    char workdir[1000];

    // getcwd(workdir, 1000);

    // cout << workdir << endl;

    dir = opendir(startdir);

    if (dir == NULL)
    {
        cout << "" << endl;
        return;
    }

    for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
    {
        char *name = entry->d_name;

        if (strcmp(name, filetofind) == 0)
        {
            result = startdir;
            cout << "found it" << endl;
            return;
        }

        cout << entry->d_name << endl;
        // cout << (int)entry->d_type << endl;
        // showstat(entry->d_name);
        // directory = 4
        // search all dirs = 1 means seach all dirs
        if (entry->d_type == 4 && search_in_all_subdirs == 1 && entry->d_name[0] != '.')
        {
            strcat(startdir, "/");
            // cout << startdir << endl;
            strcat(startdir, entry->d_name);
            // *startdir += *entryname;

            // cout << startdir << endl;

            findfile(filetofind, startdir, result, search_in_all_subdirs);
        }
    }

    closedir(dir);
    return;
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