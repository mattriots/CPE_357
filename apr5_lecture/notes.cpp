#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>

using namespace std;

int main()
{

    // tagBITMAPFILEHEADER fh;
    // cout << p[40] << endl;
    // fread(&fh, sizeof(fh), 1, file); //

    // fread(&fh.bfType, sizeof(fh.bfType), 1, file); //
    // fread(&fh.bfSize, sizeof(fh.bfSize), 1, file); //
    // fread(&fh.bfReserverd1, sizeof(fh), 1, file);               //
    // fread(&fh.bfReserverd2, sizeof(fh), 1, file);               //
    // fread(&fh.bfOffBits, sizeof(fh), 1, file);               //
    // fread(&fh, sizeof(fh), 1, file);               //

    // fclose(file);

    // int i[100];

    // int *p = new int[100];
    int *pc = (int*)malloc(sizeof(int) * 100);

    int *p = (int*) sbrk(100 * sizeof(int));

    // cout << p[40] << endl;
    cout << p << endl;
    cout << pc << endl;
    cout << sbrk(0) << endl;


    //using brk
    int *p2 = (int*)sbrk(0);
    brk(p2 + 100*sizeof(int)); // same as sbrk from above
    p2[50] = 1234;
    cout << p2[50] << endl;
    cout << p2 << endl;

    //Using sbrk to allocate space for the memory of the image
    //


    return 0;
}