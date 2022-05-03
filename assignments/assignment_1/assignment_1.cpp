#include <stdio.h>
#include <iostream>
#include <unistd.h>

using namespace std;

typedef unsigned char BYTE;

BYTE *startofheap = NULL;

BYTE *mymalloc(int size);
BYTE *myfree(BYTE *addr);
void analyze();

struct chunkinfo
{
    int size;          // size of the complete chunk INCLUDE chunkinfo
    int inuse;         // free? 0 for free, 1 for occupied
    BYTE *next, *prev; // address of next and prev chunk
};

chunkinfo *get_last_chunk();

int main()
{
    cout << &startofheap << endl;
    BYTE *a[100];
    // for(int i = 0; i < 10; i++){
    //    a[i] = mymalloc(1000);
    // }
    // analyze();
    a[1] = mymalloc(1000);
    // analyze();
    a[2] = mymalloc(1000);
    // analyze();
    a[3] = mymalloc(1000);
    a[4] = mymalloc(1000);
    a[5] = mymalloc(1000);
    analyze();
    myfree(a[2]);
    myfree(a[4]);
    myfree(a[3]);
    analyze();
}

BYTE *mymalloc(int size)
{
    size = 4096;

    // Very first chunk
    if (!startofheap)
    {

        // Set the start of the heap to be the address of the first spot
        // Then the program break moves down "size" amount
        startofheap = (BYTE *)sbrk(size);
        chunkinfo *ch = (chunkinfo *)startofheap; // Reservers 24 bytes
                                                  // at top of new memblock for chunkinfo

        // Set all the vars within chunkinfo
        ch->size = size;
        ch->inuse = 1;
        ch->next = NULL;
        ch->prev = NULL;

        // Returns the address of the startof chunk AFTER chunkinfo
        return startofheap + sizeof(chunkinfo);
    }
    else
    {

        // So far this just makes new chunks at the end of the current chunk
        BYTE *chunk = (BYTE *)sbrk(size);
        chunkinfo *last = get_last_chunk();
        chunkinfo *ch = (chunkinfo *)chunk;

        last->next = chunk;

        ch->size = size;
        ch->inuse = 1;
        ch->prev = (BYTE *)last;
        ch->next = NULL;

        return chunk + sizeof(chunkinfo);
    }
}

BYTE *myfree(BYTE *addr)
{

    chunkinfo *ch = (chunkinfo *)addr - 1;
    ch->inuse = 0;
    chunkinfo *prev = (chunkinfo *)ch->prev;
    chunkinfo *next = (chunkinfo *)ch->next;

    BYTE * above = addr - 4096;
    BYTE * below = addr + 4096;
    chunkinfo *abo = (chunkinfo *)above - 1;
    chunkinfo *bel = (chunkinfo *)below - 1;

    // chunk above and below are free
    if (abo->inuse == 0 && bel->inuse == 0)
    {
        cout << "before and after is free" << endl;
    }
    // chunk before is free
    if (abo->inuse == 0)
    {
        cout << "before is free" << endl;
        //join it with the one above it
        //combine their sizes
        //make sure it links to the right one
    }
    // chunk after is free
    if (bel->inuse == 0)
    {
        cout << " after is free" << endl;
    }

    prev->next = ch->next;
    next->prev = ch->prev;

    // 2. merge with after
    // 3. merge with before and after
    // 4. is it at the end of the list
    /////yes? sbrk - p->size;
}

chunkinfo *get_last_chunk() // you can change it when you aim for performance
{
    if (!startofheap) // I have a global void *startofheap = NULL;
        return NULL;
    chunkinfo *ch = (chunkinfo *)startofheap;
    for (; ch->next; ch = (chunkinfo *)ch->next)
        ;
    return ch;
}

void analyze()
{
    printf("\n--------------------------------------------------------------\n");
    if (!startofheap)
    {
        printf("no heap");
        return;
    }
    chunkinfo *ch = (chunkinfo *)startofheap;
    for (int no = 0; ch; ch = (chunkinfo *)ch->next, no++)
    {
        printf("%d | current addr: %x |", no, ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->inuse);
        printf("next: %x | ", ch->next);
        printf("prev: %x", ch->prev);
        printf(" \n");
    }
    printf("program break on address: %x\n", sbrk(0));
}