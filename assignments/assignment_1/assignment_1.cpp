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

chunkinfo *get_last_chunk();

int main()
{
    cout << &startofheap << endl;
    BYTE *a[100];
    // for (int i = 0; i < 8; i++)
    // {
    //     a[i] = mymalloc(1000);
    // }
    a[1] = mymalloc(10000);
    a[2] = mymalloc(400);
    a[3] = mymalloc(3);
    a[4] = mymalloc(95304);
    a[5] = mymalloc(1000);
    analyze();
    myfree(a[1]);
    myfree(a[2]);
    myfree(a[3]);
    myfree(a[5]);
    analyze();

    // BYTE *addr = mymalloc(100);

    // cout << *addr << endl;

    // analyze();

    // BYTE *a[100];
    // analyze(); // 50% points
    // for (int i = 0; i < 100; i++)
    //     a[i] = mymalloc(1000);
    // for (int i = 0; i < 90; i++)
    //     myfree(a[i]);
    // analyze();
    // myfree(a[95]);
    // a[95] = mymalloc(1000);
    // analyze();
    // for (int i = 90; i < 100; i++)
    //     myfree(a[i]);
    // analyze(); // 25% should be

    // BYTE *a[100];
    // clock_t ca, cb;
    // for (int i = 0; i < 100; i++)
    //     a[i] = mymalloc(1000);
    // for (int i = 0; i < 90; i++)
    //     myfree(a[i]);
    // myfree(a[95]);
    // a[95] = mymalloc(1000);
    // for (int i = 90; i < 100; i++)
    //     myfree(a[i]);
    // cb = clock();
    // printf("\nduration: % f\n", (double)(cb - ca));
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

    int min = 500000;

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

            BYTE *other = (BYTE *)m + realneed;
            chunkinfo *o = (chunkinfo *)other;
            o->size = m->size - realneed;
            o->inuse = 0;
            o->prev = (BYTE *)m;
            o->next = m->next;

            m->size = realneed;
            m->next = (BYTE *)o;
        }
        m->inuse = 1;
        return (BYTE *)m + sizeof(chunkinfo);
    }

    // So far this just makes new chunks at the end of the current chunk
    BYTE *chunk = (BYTE *)sbrk(realneed);
    // This gets the last chunk in the list

    // chunkinfo *last = get_last_chunk();
    chunkinfo *ch = (chunkinfo *)chunk;

    // last->next = chunk;
    st->next = chunk;

    ch->size = realneed;
    ch->inuse = 1;
    ch->prev = (BYTE *)st;
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