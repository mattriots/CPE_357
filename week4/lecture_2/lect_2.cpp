
#include <stdio.h>
#include <iostream>
#include <string.h>

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

struct listelem{
    int size;
    int occ;
    BYTE* next, prev;
}




BYTE* memstart = NULL;

BYTE *mymalloc(int size)
{
    if (memstart = NULL){

        memstart = (BYTE*) sbrk(size);
        listelem *p = (listelem*)memstart; //Reserving 24 bytes at the
                                            //top of mem block for listelem
        p -> size = size;
        p-> occ = 1;
        p->next = p->prev = NULL;
        return memstart + sizeof(listelem);
    
    }

     listelem *p = (listelem*)memstart;
    for(; p!= NULL; p = p->next)
    {
        // two questions!
    }

    //you have found nothing
    //use sbrk, have a new elem, and link it as last elem
    


}

int a[100];
int *p = a;
a[0] = 9;
a[1] = 10;
a[2] = 11;
p+= 2;
cout << p;








void myfree(void* addr){
    listelem *p = (listelem*)addr - 1; // p* - 24 bytes
    p->occ = 0;
    //1. merge with before
    //2. merge with after
    //3. merge with before and after
    //4. is it at the end of the list
    /////yes? sbrk - p->size;
}

int main()
{
}