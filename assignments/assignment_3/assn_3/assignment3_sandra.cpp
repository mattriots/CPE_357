#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <time.h>
#include <string.h>
#include <signal.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>
using namespace std;

int fd[2];

void signalhandler(int i)
{
    dup2(fd[0], STDIN_FILENO);

}

//char* filename_to_be_found, 
//char *startdirectory,
//char *result,
//int search_in_all_subdirectories
int findstuff(DIR *dir, struct dirent* entry, char *filename, int fs, int found, char *directory, char *childrep)
{
    while((entry = readdir(dir)) != NULL)
    {
        //some changes 
        if(strstr(".", entry->d_name) || strstr("..", entry->d_name) != 0)
        {
            //continue;
        }
        else if(fs)
        {
            // if((fileset == 1) && (strstr(entry->d_name, ftype) == 0))
            // {
            //     //continue;
            // }
            if(strncmp((entry->d_name), filename, strlen(filename)) == 0)
            {
                found = 1;
                strcat(childrep,"Found file: ");
                strcat(childrep, entry->d_name);
                strcat(childrep, " in the subdirectory-> ");
                strcat(childrep, directory);
                strcat(childrep, "\n");
            }
            // 2. is the entry a directory? recursion!
            else if (entry->d_type == DT_DIR) 
            {
                char cwd[5000];
                strcpy(cwd, directory);
                strcat(cwd, "/");
                strcat(cwd, entry->d_name);

                DIR *dir2 = opendir(cwd);
                found += findstuff(dir2, entry, filename, fs, found, cwd, childrep);
            }
        }
    }
    return found;
}

//fileset and ftype might not be necessary

int main()
{
    int PID = getpid();
    int *childPID = (int*)mmap(NULL, (10*sizeof(int)), PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
    int *child_num = (int*) mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
    int *child_done = (int*) mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
    char *inputs = (char*) mmap(NULL, 1000, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
    char *buffer= (char*) malloc(1064748124);
    
    *child_done = 0;

    signal(SIGUSR1, signalhandler);
    int save_stdin = dup(STDIN_FILENO);
    pipe(fd);
    
    cout<<"===================== User directory ====================="<<endl;
    cout<<"1) find <filename> = search file in the current directory"<<endl;
    cout<<"2) find <filename> -s = search file in the subdirectory"<<endl;
    cout<<"3) find <filename> -f = search file in all directory"<<endl;
    cout<<"4) q or quit = end of program"<<endl;
    cout<<"==========================================================="<<endl;

    fflush(0);

    while(1)
    {
        cout<<"findstuff$: ";
        fflush(0);
        dup2(save_stdin, STDIN_FILENO);
        
        int i = 0;

        while(buffer[i] != 0)
        {
            buffer[i] = 0;
            i++;
        }

        read(STDIN_FILENO, buffer, 1064748124);
        fflush(0);

        if(*child_done)
        {
            printf("\n%s\n", buffer);
            fflush(0);
            *child_done = 0;
        }

        if(strchr(buffer, 'q') != 0 || strncmp(buffer, "quit", 4) == 0)
        {
            fflush(0);
            for(int i = 0; i< 10; i++) //10 is the limitation
            {
                if(childPID[i] != 0)
                {
                    kill(childPID[i], SIGKILL);
                }
            }
            cout<<endl;
            cout<<"End of Program"<<endl;

            munmap(&childPID, 10*sizeof(int));
            munmap(&inputs, 1000);
            munmap(&child_num, 4);
            munmap(&child_done, 4);
            free(buffer);
            wait(0);

            return 0;
        }
        else if (strncmp("find", buffer, 4) == 0)
        {
            int sub_dir = 0;

            //If 
            if(*child_num == 9)
            {
                write(STDOUT_FILENO, "The number of thread allowed has been reached it's limit of 10.", 31);
                fsync(fd[1]);
            }
            else
            {
                (*child_num)++;
                if(fork() == 0)
                {
                    int found = 0;
                    int a = 0;
                    for(;a < 10; a++)
                    {
                        if(childPID[a] == 0)
                        {
                            child_num[a] = getpid();
                            strcpy(inputs + a*100, buffer);
                            break;
                        }
                    }

                    if(strstr(buffer, "-s") != 0) //user enter -s = search in subdirectory
                    {
                        sub_dir = 1;
                    }

                    //char ftype[5];
                    //int fileset = 0;

                    //if(strstr(buffer, "-f") != 0) //user enter -f = search in all directory

                    char filename [100];
                    int fs = 0;
                    if ( strchr(buffer, '.') != 0 )
                    {
                        fs = 1;
                        int k = 0;
                        while (buffer[k] != ' ')
                        {
                            k++;
                        }
                        k++;

                        int j = 0;
                        while(buffer[k] != '\n' && buffer[k] != ' ' && buffer[k] != '\0')
                        {
                            filename[j] = buffer[k];
                            k++;
                            j++;
                        }
                        filename[j] = '\0';
                        filename[j+1] = '\0';
                    }

                    DIR *dir = opendir(getcwd(NULL, 5000));
                    struct dirent *entry;
                    char childrep[100000];
                    strcpy(childrep, "");

                    if(sub_dir)
                    {
                        found  = findstuff(dir, entry, filename, fs, found, getcwd(NULL, 5000), childrep);
                    }

                    while((entry = readdir(dir)) != NULL && !sub_dir)
                    {
                        if(fs)
                        {
                            if(strstr(".", entry->d_name)!=0 || strstr("..", entry->d_name)!=0)
                            {
                                //continue;
                            }
                            else if (strncmp((entry->d_name), filename, strlen(filename)) == 0)
                            {
                                found = 1;
                                char *cwd;
                                cwd = getcwd(NULL, 5000);
                                strcat(childrep,"Found file: ");
                                strcat(childrep, entry->d_name);
                                strcat(childrep, " in the directory-> ");
                                strcat(childrep, cwd);
                                strcat(childrep, "\n");
                            }
                        }
                    }

                    if(found == 0)
                    {
                        char *cwd;
                        cwd = getcwd(NULL, 5000);
                        strcpy(childrep, "File Not Found");
                    }

                    fflush(0);
                    closedir(dir);
                    childPID[a] = 0;
                    inputs[a*100] = 0;

                    close(fd[0]);
                    write(fd[1], childrep, strlen(childrep));
                    fsync(fd[1]);
                    fsync(STDOUT_FILENO);
                    close(fd[1]);
                    *child_done = 1;
                    kill(PID, SIGUSR1);
                    (*child_num)-- ;
                    
                    return 0;
                }
            }
        }

    }

    return 0;

}