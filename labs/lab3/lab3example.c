#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/time.h>


void longTask()
{
    sleep(1);
    printf("Task complete\n");
}


// Example of how to get duration in C, don't use clock_t
void timeDurationExample()
{
    printf("Starting clock ...\n");
    struct timeval start, end;
    gettimeofday(&start, NULL);
    sleep(2);
    gettimeofday(&end, NULL);
    long duration = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);
    printf("This process took: %ld microseconds\n", duration);
    // Duration should be close to two million in this example
}


int main(int argc, char **argv)
{
    timeDurationExample();
    //This takes too long!
    //longTask();
    //longTask();
    // To solve? Fork()!
  

    // This number is now shared between parent and kid! 
    // For your lab3, this would be your data array! 
    int *number = (int*) mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0); 
    int pid = fork();

    if (pid == 0)
    {
        // Child
        *number = 7;
        printf("In child, the number is: %d\n", *number);

        // Need to make sure child returns! Other wise it will keep executing
        return 0;
    }
    else
    {
        // Parent
        // Don't wait here at the top! (see more below)
        // wait(NULL);  <- Not okay
        sleep(1);
        // Printing number in the parent, value should be the same! -> 7
        printf("In parent, the number is: %d\n", *number);
        // You MUST wait for the child to finish first in your lab!
        // If the parent is done and it tries to write a new BMP while
        // the child is still calculating, your image will be screwed up!
        // You wait AFTER the parent does it's work, not before.
        // If you wait for the kid to terminate before you start parent,
        // then why fork in the first place?
        wait(NULL);
    }

    // Don't forget to free your memory
    munmap(number, sizeof(int));
    return 0;
}
