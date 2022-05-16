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

using namespace std;

void showstat(char *filepath);

int main()
{

    char workdir[1000];

    getcwd(workdir, 1000); // get current work director
                           // getcwd(target, size).
    cout << "current directory: " << workdir << endl;

    DIR *dir;
    struct dirent *entry;

    //Put all thsi in a funtion and call it recursiveley
    //to go through all the sub folders

    dir = opendir(".");
    if (dir == NULL)
    {
        cout << "bad case" << endl;
        return 0;
    }

    // entry = the directory...current?
    // While entry is not null keep assigning the current dir to entry
    //.....I think.....
    // when opendir() is succesful
    // readdir() will read one entry at a time, then next, then next
    for (entry = readdir(dir); entry != NULL; entry = readdir(dir))
    {
        cout << "-----------------------" << endl;
        cout << entry->d_name << endl;
        cout << (int)entry->d_type << endl;
        showstat(entry->d_name);
        // dir = opendir("./foldername");
    }

    return 0;
}

// Making a function to hold all of the stats
// Will pass in a filename
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

    // printf("I-node number:            %ju\n", (uintmax_t)sb.st_ino);

    // printf("Mode:                     %jo (octal)\n",
    //        (uintmax_t)sb.st_mode);

    // printf("Link count:               %ju\n", (uintmax_t)sb.st_nlink);
    // printf("Ownership:                UID=%ju   GID=%ju\n",
    //        (uintmax_t)sb.st_uid, (uintmax_t)sb.st_gid);

    // printf("Preferred I/O block size: %jd bytes\n",
    //        (intmax_t)sb.st_blksize);
    // printf("File size:                %jd bytes\n",
    //        (intmax_t)sb.st_size);
    // printf("Blocks allocated:         %jd\n",
    //        (intmax_t)sb.st_blocks);

    // printf("Last status change:       %s", ctime(&sb.st_ctime));
    // printf("Last file access:         %s", ctime(&sb.st_atime));
    // printf("Last file modification:   %s", ctime(&sb.st_mtime));
}