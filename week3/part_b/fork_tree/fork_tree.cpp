#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

#include <sys/wait.h>

using namespace std;

typedef unsigned char BYTE;

int main()
{

    // if (fork() == 0) //child
    // {
    //     if (fork() == 0)//child of child 
    //     {   
    //         cout << "A"  << endl; 

    //         return 0;
    //     }
    //     wait(0); // waits for child of child to finish

    //      cout << "B"  << endl;

    //     return 0;
    // }
    // else //parent 
    // {
    //      wait(0); // waits for child to finish

    //      cout << "C"  << endl;
    // }

   if(fork() == 0){ // child 1
       printf("C");
       return 0;
   }
   else // main parent
   {
       wait(0);

       if(fork() ==0){ //child 2
           printf("D");
           return 0;
       }
       else //sub parent
       {
           wait(0);
           printf("A");
       }
       printf("B");

   }

    return 0;
}