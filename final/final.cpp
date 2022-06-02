#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

#include <sys/mman.h>

using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

struct tagBIGMAPFILEHEADER
{
    WORD bfType;      // specifies file type
    DWORD bfSize;     // specifies the size in bytes of bitmap file
    WORD bfReserved1; // reserved: must be 0;
    WORD bfReserved2; // reserved; butst be 0;
    DWORD bfOffBits;  // specifies the offset in bytes from the bitmap
                      // filehead to bitmap bits
};

struct tagBITMAPINFOHEADER
{
    DWORD biSize;         // specifies the number of bytes required by struct
    LONG biWidth;         // specifies width in pixels
    LONG biHeight;        // specifies height in pixels
    WORD biPlanes;        // specifies the number of color planes, must be 1
    WORD biBitCount;      //# of bits per pixel
    DWORD biCompression;  // type of compression
    DWORD biSizeImage;    // size of image in bytes
    LONG biXPelsPerMeter; // number of pixels per meter in x axis
    LONG biYPelsPerMeter; // number of pixels per meter in y axis
    DWORD biClrUsed;      // number of colors used by bitmap
    DWORD biClrImportant; // number of colors that are important
};

void inFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, FILE *fileIn);
void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *pix, char *fileOut);
void multiply(int par_id, int par_count, float *pix1, float *pix2, float *datastore, int width, struct timeval *start);
void normalize(BYTE *pix, float *pixStore, int size);
void finalize(float *datastore, BYTE *resultstore, int size);
void synch(int par_id, int par_count, int *ready, int ri);

int main(int argc, char **argv)
{

    ////////////////////////////////////
    // VARIABLES, SETUP, CLI INPUT    //
    ///////////////////////////////////

    int par_id = 0;                           // the parallel ID of this process
    int par_count;                            // the amount of processes
    float *pix1Store, *pix2Store, *datastore; // matrices A,B and C
    int *ready, *size, *width;                // needed for synch
    char *fileInName1, *fileInName2, *fileOut;

    BYTE *resultstore;
    tagBIGMAPFILEHEADER fh1; // Struct var for first fileheaders
    tagBITMAPINFOHEADER fih1;

    tagBIGMAPFILEHEADER fh2; // Struct var for second fileheaders
    tagBITMAPINFOHEADER fih2;

    struct timeval start, end;

    if (argc != 3)
    {
        printf("no shared\n");
    }
    else
    {
        par_count = atoi(argv[1]);
        par_id = atoi(argv[2]);
    }

    int fd[6];
    if (par_id == 0)
    {

        fileInName1 = "f1.bmp";
        fileInName2 = "f2.bmp";
        fileOut = "result.bmp";

        FILE *fileIn1 = fopen(fileInName1, "rb"); // Open first pic
        inFile(fh1, fih1, fileIn1);               // Read in all the header data from file

        FILE *fileIn2 = fopen(fileInName2, "rb"); // Open small pic
        inFile(fh2, fih2, fileIn2);               // Read in all the header data from file

        int isize1, width1;

        // Assign size | width | height

        isize1 = fih1.biSizeImage;
        width1 = fih1.biWidth;

        resultstore = (BYTE *)malloc(isize1);

        // TODO: init the shared memory for A,B,C, ready. shm_open with C_CREAT here! then ftruncate! then mmap
        fd[0] = shm_open("size", O_CREAT | O_RDWR, 0777);
        fd[1] = shm_open("width", O_CREAT | O_RDWR, 0777);
        fd[2] = shm_open("matrixA", O_CREAT | O_RDWR, 0777);
        fd[3] = shm_open("matrixB", O_CREAT | O_RDWR, 0777);
        fd[4] = shm_open("matrixC", O_CREAT | O_RDWR, 0777);
        fd[5] = shm_open("synchobject", O_CREAT | O_RDWR, 0777);

        ftruncate(fd[0], sizeof(int));
        ftruncate(fd[1], sizeof(int));
        ftruncate(fd[2], isize1 * sizeof(float));
        ftruncate(fd[3], isize1 * sizeof(float));
        ftruncate(fd[4], isize1 * sizeof(float));
        ftruncate(fd[5], par_count * sizeof(int));

        size = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        width = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
        pix1Store = (float *)mmap(NULL, isize1 * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
        pix2Store = (float *)mmap(NULL, isize1 * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);
        datastore = (float *)mmap(NULL, isize1 * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[4], 0);
        ready = (int *)mmap(NULL, par_count * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[5], 0);

        // ready[n] = 0; // set all the ready[par_id] to 0
        for (int i = 0; i < par_count; i++)
        {
            ready[i] = 0;
        }

        // Space for the 2 picture pixels
        BYTE *pix1 = (BYTE *)malloc(isize1);
        BYTE *pix2 = (BYTE *)malloc(isize1);

        // Assign size and width for use in the other processes

        *size = isize1;
        *width = width1;

        fread(pix1, isize1, 1, fileIn1); // Read in pix data
        fread(pix2, isize1, 1, fileIn2); // Read in pix data

        fclose(fileIn1); // Close file
        fclose(fileIn2); // Close file

        normalize(pix1, pix1Store, isize1);
        normalize(pix2, pix2Store, isize1);

        // Close them and free once we turn them into float arrays
        free(pix1);
        free(pix2);
    }
    else
    {
        sleep(2);
        fd[0] = shm_open("size", O_CREAT | O_RDWR, 0777);
        fd[1] = shm_open("width", O_CREAT | O_RDWR, 0777);
        fd[2] = shm_open("matrixA", O_CREAT | O_RDWR, 0777);
        fd[3] = shm_open("matrixB", O_CREAT | O_RDWR, 0777);
        fd[4] = shm_open("matrixC", O_CREAT | O_RDWR, 0777);
        fd[5] = shm_open("synchobject", O_CREAT | O_RDWR, 0777);

        size = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        width = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
        pix1Store = (float *)mmap(NULL, *size * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
        pix2Store = (float *)mmap(NULL, *size * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);
        datastore = (float *)mmap(NULL, *size * sizeof(float), PROT_READ | PROT_WRITE, MAP_SHARED, fd[4], 0);
        ready = (int *)mmap(NULL, par_count * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[5], 0);
    }

    synch(par_id, par_count, ready, 0);

    // Only do parallel processes here

    multiply(par_id, par_count, pix1Store, pix2Store, datastore, *width, &start);

    // end paralell here ?

    synch(par_id, par_count, ready, 1);

    if (par_id == 0)
    {
        gettimeofday(&end, NULL);

        long duration = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);

        printf("time taken: %d\n", duration);

        finalize(datastore, resultstore, *size);

        outFile(fh1, fih1, resultstore, fileOut);
    }

    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    close(fd[4]);
    close(fd[5]);

    shm_unlink("size");
    shm_unlink("width");
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");

    munmap(size, sizeof(int));
    munmap(width, sizeof(int));
    munmap(pix1Store, *size * sizeof(float));
    munmap(pix2Store, *size * sizeof(float));
    munmap(datastore, *size * sizeof(float));
    munmap(ready, par_count * sizeof(int));

    free(resultstore);

    return 0;
}

void synch(int par_id, int par_count, int *ready, int ri) // ready[n]
{

    ready[par_id]++;
    int leave = 1;
    while (leave)
    {
        leave = 0;
        for (int i = 0; i < par_count; i++)
        {
            if (ready[i] <= ri)
            {
                leave = 1;
            }
        }
    }
}

void normalize(BYTE *pix, float *pixStore, int size)
{
    for (int i = 0; i < size; i++)
    {
        float temp = (float)pix[i];
        temp = temp / 255;
        pixStore[i] = temp;
    }
}

void finalize(float *datastore, BYTE *resultstore, int size)
{

    for (int i = 0; i < size; i++)
    {
        float temp = datastore[i];
        temp = temp * 255;
        temp = temp * 0.03;
        resultstore[i] = (BYTE)temp;
    }
}

void multiply(int par_id, int par_count, float *pix1, float *pix2, float *datastore, int width, struct timeval *start)
{
    if (par_id == 0)
    {
        for (int a = 0; a < width; a++)
            for (int b = 0; b < width; b++)
                datastore[a + b * width] = 0.0;

        gettimeofday(start, NULL);
    }
    // int low = width * par_id / par_count;
    // int high = (width * (par_id + 1) / par_count);

    // printf("par_id: %d from: %d to: %d\n",par_id, low, high);

    // multiply
    for (int a = 0; a < width; a++)                                                             // over all cols a
        for (int b = (width * par_id / par_count); b < (width * (par_id + 1) / par_count); b++) // over all rows b
            for (int c = 0; c < width; c++)                                                     // over all rows/cols left
            {

                datastore[a * 3 + 0 + b * width * 3] += pix1[c * 3 + 0 + b * width * 3] * pix2[a * 3 + 0 + c * width * 3];
                datastore[a * 3 + 1 + b * width * 3] += pix1[c * 3 + 1 + b * width * 3] * pix2[a * 3 + 1 + c * width * 3];
                datastore[a * 3 + 2 + b * width * 3] += pix1[c * 3 + 2 + b * width * 3] * pix2[a * 3 + 2 + c * width * 3];
            }
}

void inFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, FILE *inFile)
{
    fread(&fh.bfType, sizeof(fh.bfType), 1, inFile);
    fread(&fh.bfSize, sizeof(fh.bfSize), 1, inFile);
    fread(&fh.bfReserved1, sizeof(fh.bfReserved1), 1, inFile);
    fread(&fh.bfReserved2, sizeof(fh.bfReserved2), 1, inFile);
    fread(&fh.bfOffBits, sizeof(fh.bfOffBits), 1, inFile);

    fread(&fih.biSize, sizeof(fih.biSize), 1, inFile);
    fread(&fih.biWidth, sizeof(fih.biWidth), 1, inFile);
    fread(&fih.biHeight, sizeof(fih.biHeight), 1, inFile);
    fread(&fih.biPlanes, sizeof(fih.biPlanes), 1, inFile);
    fread(&fih.biBitCount, sizeof(fih.biBitCount), 1, inFile);
    fread(&fih.biCompression, sizeof(fih.biCompression), 1, inFile);
    fread(&fih.biSizeImage, sizeof(fih.biSizeImage), 1, inFile);
    fread(&fih.biXPelsPerMeter, sizeof(fih.biXPelsPerMeter), 1, inFile);
    fread(&fih.biYPelsPerMeter, sizeof(fih.biYPelsPerMeter), 1, inFile);
    fread(&fih.biClrUsed, sizeof(fih.biClrUsed), 1, inFile);
    fread(&fih.biClrImportant, sizeof(fih.biClrImportant), 1, inFile);
}

void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *dataStore, char *fileOut)
{
    FILE *outFile = fopen(fileOut, "wb");

    fwrite(&fh.bfType, sizeof(fh.bfType), 1, outFile);
    fwrite(&fh.bfSize, sizeof(fh.bfSize), 1, outFile);
    fwrite(&fh.bfReserved1, sizeof(fh.bfReserved1), 1, outFile);
    fwrite(&fh.bfReserved2, sizeof(fh.bfReserved2), 1, outFile);
    fwrite(&fh.bfOffBits, sizeof(fh.bfOffBits), 1, outFile);

    fwrite(&fih.biSize, sizeof(fih.biSize), 1, outFile);
    fwrite(&fih.biWidth, sizeof(fih.biWidth), 1, outFile);
    fwrite(&fih.biHeight, sizeof(fih.biHeight), 1, outFile);
    fwrite(&fih.biPlanes, sizeof(fih.biPlanes), 1, outFile);
    fwrite(&fih.biBitCount, sizeof(fih.biBitCount), 1, outFile);
    fwrite(&fih.biCompression, sizeof(fih.biCompression), 1, outFile);
    fwrite(&fih.biSizeImage, sizeof(fih.biSizeImage), 1, outFile);
    fwrite(&fih.biXPelsPerMeter, sizeof(fih.biXPelsPerMeter), 1, outFile);
    fwrite(&fih.biYPelsPerMeter, sizeof(fih.biYPelsPerMeter), 1, outFile);
    fwrite(&fih.biClrUsed, sizeof(fih.biClrUsed), 1, outFile);
    fwrite(&fih.biClrImportant, sizeof(fih.biClrImportant), 1, outFile);

    int isize = fih.biSizeImage;
    fwrite(dataStore, isize, 1, outFile);
    fclose(outFile);
}
