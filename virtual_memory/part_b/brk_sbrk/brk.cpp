#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define PAGESIZE 4096 // 4;
typedef unsigned char BYTE;

int main(int argc, char *argv[])
{

    void *p;

    int i = brk(p);
                //brk(pointer) <--- requires an address pointer
                //brk(p) returns an int;
                // 0 = everything good
                // -1 = something is wrong
                //errno is used to check what error is

    int s = 4096;
    void *a = sbrk(s);
                //sbrk(int size) <---size of increase we want
                //sbrk(s) returns address of PREVIOUS program break
                    //before the new size was added. aka where the
                    //new 4kb starts. similar to malloc

    void *b = sbrk(0); //Returns current location of
                        // program break

    brk(a + s); //sbrk(s) equivalent

    //-----------------------------------------------------

    void *p;         //UNINT pointer *p

    void *a = sbrk(0); //pointer *a = start of page break
    sbrk(PAGESIZE);    //Allocate 4k space
    a = sbrk(0);       //Increments by 4k or 0x1000
    sbrk(-PAGESIZE);   //Shrink it by 4K
    a = sbrk(0);        //Back to OG

//     //------------------------------------------------------

    void *p;

    BYTE *a = (BYTE *)sbrk(0);
    printf("%p\n",(void*) a);
    a += PAGESIZE;              //a = a + PAGESIZE
    brk(a);                     //brk(a) increases page break by 4k (PAGESIZE
    a = (BYTE *)sbrk(0);        //Checking current location
   printf("%p\n",(void*) a);
    // sbrk(-PAGESIZE);            // Shrink it by 4K
     a -= PAGESIZE;                // Shrink it by 4K
     brk(a);                  //brk(a) decreases break by 4k (PAGESIZE)
    a = (BYTE *)sbrk(0);        //Checking current location again
   printf("%p\n",(void*) a);     // Back to OG


//     //---------------------------------------------------



    return 0;
}