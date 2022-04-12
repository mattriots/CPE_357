#include <stdio.h>
#include <string.h>
#include <iostream>

#include <sys/mman.h> //for mmap

using namespace std;

typedef unsigned char BYTE;



// //////FLAGS///////

// #define MYFLAG_A 1 //0000 0001
// #define MYFLAG_B 2 //0000 0010
// #define MYFLAG_C 4 //0000 0100
// #define MYFLAG_D 8 //0000 1000


// void fct(int flag){
//     ....

// }

// int main(){

//     fct(MYFLAG_A | MYFLAG_D); //0000 1001
//                             //Can pass multiple flags with or '|' operator
//     mmap(NULL, 4,)


//     return 0;
// }

/////////////////////////////////////////


int main(){

    // BYTE * p = (BYTE* )mmap(NULL, 4,PROT_READ | PROT_WRITE, 
    // MAP_SHARED | MAP_ANONYMOUS, -1,0);
    //                                     //mmap(
    //                                     // Recommended address, 
    //                                     // How many bytes, 
    //                                     // Protection level Read or write?, 
    //                                     // Map shared or Map Anon,
    //                                     // file pointer or -1,
    //                                     // offest or  0)
    //                                     // PROT_READ = protected read
    //                                     // PROT_WRITE = protected write
    //                                     // MAP_SHARED = sharing file with other process
    //                                     // MAP_ANON = Use if not calling a file pointer
    //                                     // 

    //  munmap(p,4);           //Removes alloted space
    //                         //munmap (address, size)                       

    char *text;
    strcpy(text, hello);            
                                        
    return 0;
}


