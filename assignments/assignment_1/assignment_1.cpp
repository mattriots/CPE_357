#include <stdio.h>
#include <iostream>
#include <unistd.h>

using namespace std;

#define PAGESIZE 4096
typedef unsigned char BYTE;

BYTE *startofheap = NULL;

BYTE *mymalloc(int size);
void myfree(BYTE *addr);
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
    // cout << &startofheap << endl;
    // BYTE *a[100];
    // // // // for(int i = 0; i < 10; i++){
    // // // //    a[i] = mymalloc(1000);
    // // // // }
    // a[1] = mymalloc(1000);
    // a[2] = mymalloc(1000);
    // a[3] = mymalloc(1000);
    // a[4] = mymalloc(1000);
    // // a[5] = mymalloc(1000);
    // analyze();
    // myfree(a[1]);
    // myfree(a[2]);
    // myfree(a[4]);
    // myfree(a[3]);
    // analyze();

    BYTE *a[100];
    analyze(); // 50% points
    for (int i = 0; i < 100; i++)
        a[i] = mymalloc(1000);
    for (int i = 0; i < 90; i++)
        myfree(a[i]);
    analyze();
}

BYTE *mymalloc(int size)
{

    // Very first chunk
    if (!startofheap)
    {

        // Set the start of the heap to be the address of the first spot
        // Then the program break moves down "size" amount
        startofheap = (BYTE *)sbrk(PAGESIZE);
        chunkinfo *ch = (chunkinfo *)startofheap; // Reservers 24 bytes
                                                  // at top of new memblock for chunkinfo

        // Set all the vars within chunkinfo
        ch->size = PAGESIZE;
        ch->inuse = 1;
        ch->next = NULL;
        ch->prev = NULL;

        // Returns the address of the startof chunk AFTER chunkinfo
        return startofheap + sizeof(chunkinfo);
    }

    // So we need to search through all the mem we have so far
    // And if there is any room to fit the requested size mem
    // If so we want to find the smallest possible spot to put it
    //

    // So far this just makes new chunks at the end of the current chunk
    BYTE *chunk = (BYTE *)sbrk(PAGESIZE);
    // This gets the last chunk in the list

    chunkinfo *last = get_last_chunk();
    chunkinfo *ch = (chunkinfo *)chunk;

    last->next = chunk;

    ch->size = PAGESIZE;
    ch->inuse = 1;
    ch->prev = (BYTE *)last;
    ch->next = NULL;

    return chunk + sizeof(chunkinfo);
}

// void for now...but do we want to have it return addresses of free spaces!?!?!

void myfree(BYTE *addr)
{
    // setting ch to be the beginning of chunk info: with the addr -1
    // Then sets the inuse to 0
    chunkinfo *ch = (chunkinfo *)addr - 1;
    ch->inuse = 0;

    // If this is the last mem chunk
    // Then "a" will be past the current page break
    // So we set the prev-> next to be null and move page break up
    chunkinfo *prev = (chunkinfo *)ch->prev;
    chunkinfo *next = (chunkinfo *)ch->next;

    if (next == NULL && prev != NULL && prev->inuse == 0)
    {
        prev->next = NULL;
        prev->size = prev->size + ch->size;

        (BYTE *)sbrk(-prev->size);

        if (sbrk(0) <= startofheap)
        {
            startofheap = NULL;
        }

        return;
    }

    if (next == NULL)
    {
        prev->next = NULL;

        (BYTE *)sbrk(-ch->size);

        if (sbrk(0) < startofheap)
        {
            startofheap = NULL;
        }

        return;
    }

    // Setting up vars to represent the prev and next 4k mem chunks
    // That the current chunk is CONNECTED TO...NOT next to

    // chunk above and below are free
    if (prev != NULL && prev->inuse == 0 && next->inuse == 0 && next->size != NULL && prev->size != NULL)
    {
        // cout << "before and after is free" << endl;
        prev->size = prev->size + next->size + ch->size;
        prev->next = ch->next;
        next->prev = ch->prev;

        return;
    }

    // chunk before is free
    if (prev != NULL && prev->inuse == 0 && prev->size != NULL)
    {
        // cout << "before is free" << endl;
        prev->size = prev->size + ch->size;

        prev->next = ch->next;
        next->prev = ch->prev;

        return;
    }

    // chunk after is free
    if (next != NULL && next->inuse == 0 && next->size != NULL)
    {
        // cout << " after is free" << endl;
        next->size = next->size + ch->size;

        prev->next = ch->next;
        next->prev = ch->prev;

        return;
    }

    // If it makes it thorugh all the if statements somehow we can still set the prev/next here
    // if (prev == NULL)
    // {
    //     next->prev = NULL;
    // }
    // else
    // {
    //     prev->next = ch->next;
    //     next->prev = ch->prev;
    // }
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
        printf(", program break on address: %x\n", sbrk(0));
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