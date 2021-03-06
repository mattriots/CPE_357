#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

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

int main()
{

    BYTE *a[100];
    analyze(); // 50% points
    for (int i = 0; i < 100; i++)
        a[i] = mymalloc(1000);
    for (int i = 0; i < 90; i++)
        myfree(a[i]);
    analyze();
    myfree(a[95]);
    a[95] = mymalloc(1000);
    analyze();
    for (int i = 90; i < 100; i++)
        myfree(a[i]);
    analyze();

    return 0;
}

BYTE *mymalloc(int size)
{
    int neededbytes = size + sizeof(chunkinfo);
    int pagesneed = neededbytes / PAGESIZE + 1;
    int realneed = pagesneed * PAGESIZE;

    // Very first chunk
    if (!startofheap)
    {

        // Set the start of the heap to be the address of the first spot
        // Then the program break moves down "size" amount
        startofheap = (BYTE *)sbrk(realneed);
        chunkinfo *ch = (chunkinfo *)startofheap; // Reservers 24 bytes
                                                  // at top of new memblock for chunkinfo

        // Set all the vars within chunkinfo
        ch->size = realneed;
        ch->inuse = 1;
        ch->next = NULL;
        ch->prev = NULL;

        // Returns the address of the startof chunk AFTER chunkinfo
        return startofheap + sizeof(chunkinfo);
    }

    BYTE *start = startofheap;
    chunkinfo *st = (chunkinfo *)start;

    BYTE *mem = NULL;
    chunkinfo *m = (chunkinfo *)mem;

    int min = INT32_MAX;

    for (; st->next; st = (chunkinfo *)st->next)
    {
        if (st->inuse == 0 && st->size >= realneed && st->size < min)
        {
            min = st->size;
            m = st;

            if (min == 4096)
            {
                break;
            }
        }
    }

    if (m != NULL)
    {
        if ((m->size - realneed) > 0)
        {
            // what WAS next for the first half of this split chunk
            // Need to make sure its prev is now the SECOND half of the split chunk
            chunkinfo *nextnext = (chunkinfo *)m->next;

            BYTE *other = (BYTE *)m + realneed;
            chunkinfo *o = (chunkinfo *)other;
            o->size = m->size - realneed;
            o->inuse = 0;
            o->prev = (BYTE *)m;
            o->next = m->next;

            m->size = realneed;
            m->next = (BYTE *)o;

            nextnext->prev = (BYTE *)o;
        }
        m->inuse = 1;
        return (BYTE *)m + sizeof(chunkinfo);
    }

    // this just makes new chunks at the end of the current chunk
    BYTE *chunk = (BYTE *)sbrk(realneed);

    chunkinfo *ch = (chunkinfo *)chunk;

    st->next = chunk;

    ch->size = realneed;
    ch->inuse = 1;
    ch->prev = (BYTE *)st;
    ch->next = NULL;

    return chunk + sizeof(chunkinfo);
}

void myfree(BYTE *addr)
{
    // setting ch to be the beginning of chunk info: with the addr -1
    // Then sets the inuse to 0
    chunkinfo *ch = (chunkinfo *)addr - 1;
    ch->inuse = 0;

    chunkinfo *prev = (chunkinfo *)ch->prev;
    chunkinfo *next = (chunkinfo *)ch->next;

    // Only one chunk left in existence
    if (next == NULL && prev == NULL)
    {
        // prev->next = NULL;

        (BYTE *)sbrk(-ch->size);

        if (sbrk(0) <= startofheap)
        {
            startofheap = NULL;
        }

        return;
    }

    // Its the last mem chunk
    // And the prev mem chunk is unused
    // So we can move the SBRK up
    // All the way to the PREV chunk
    if (next == NULL && prev != NULL && prev->inuse == 0)
    {
        chunkinfo *prevprev = (chunkinfo *)prev->prev;
        if (prevprev != NULL)
        {
            prevprev->next = NULL;
        }
        prev->next = NULL;
        prev->size = prev->size + ch->size;

        BYTE *f = (BYTE *)sbrk(-prev->size);
        BYTE *t = (BYTE *)sbrk(0);

        if (sbrk(0) <= startofheap)
        {
            startofheap = NULL;
        }

        return;
    }

    // Its the last mem chunk
    // And the prev mem chunk is USED
    // So we can move the SBRK up
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

    // chunk above and below are free
    if (prev != NULL && next != NULL && prev->inuse == 0 && next->inuse == 0 && next->size != NULL && prev->size != NULL)
    {
        chunkinfo *nextnext = (chunkinfo *)next->next;
        prev->size = prev->size + next->size + ch->size;
        if (nextnext != NULL)
        {
            prev->next = (BYTE *)nextnext;
            nextnext->prev = ch->prev;
        }

        // next->prev = ch->prev;

        return;
    }

    // chunk above is free
    if (prev != NULL && prev->inuse == 0 && prev->size != NULL)
    {
        // cout << "before is free" << endl;
        prev->size = prev->size + ch->size;

        prev->next = ch->next;
        next->prev = ch->prev;

        return;
    }

    // chunk below is free
    if (next != NULL && next->inuse == 0 && next->size != NULL)
    {
        chunkinfo *nextnext = (chunkinfo *)next->next;

        ch->size = next->size + ch->size;
        if (prev == NULL)
        {
            next->prev = NULL;
            ch->next = next->next;
            nextnext->prev = (BYTE *)ch;
            return;
        }

        prev->next = ch->next;
        next->prev = ch->prev;

        return;
    }
    
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