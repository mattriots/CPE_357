#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cmath>

#include <time.h>
#include <sys/mman.h>
#include <sys/wait.h>

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

int main(int argc, char **argv)
{

    char *fileIn = argv[1];
    float b_grade = stof(argv[2]);
    float g_grade = stof(argv[3]);
    float r_grade = stof(argv[4]);
    char *fileOut = argv[5];

    // char *fileIn = "jar.bmp";
    // char *fileOut = "jar_2.bmp";

    // float b_grade = 0.5;
    // float g_grade = 0.5;
    // float r_grade = 1.0;

    void outFile(tagBIGMAPFILEHEADER fh, tagBITMAPINFOHEADER fih, BYTE * pix, char *fileOut);
    void colorGrading(BYTE * pix, int size, float b_grade, float g_grade, float r_grade);

    FILE *file = fopen(fileIn, "rb");
    tagBIGMAPFILEHEADER fh;

    fread(&fh.bfType, sizeof(fh.bfType), 1, file);
    fread(&fh.bfSize, sizeof(fh.bfSize), 1, file);
    fread(&fh.bfReserved1, sizeof(fh.bfReserved1), 1, file);
    fread(&fh.bfReserved2, sizeof(fh.bfReserved2), 1, file);
    fread(&fh.bfOffBits, sizeof(fh.bfOffBits), 1, file);

    tagBITMAPINFOHEADER fih;

    fread(&fih.biSize, sizeof(fih.biSize), 1, file);
    fread(&fih.biWidth, sizeof(fih.biWidth), 1, file);
    fread(&fih.biHeight, sizeof(fih.biHeight), 1, file);
    fread(&fih.biPlanes, sizeof(fih.biPlanes), 1, file);
    fread(&fih.biBitCount, sizeof(fih.biBitCount), 1, file);
    fread(&fih.biCompression, sizeof(fih.biCompression), 1, file);
    fread(&fih.biSizeImage, sizeof(fih.biSizeImage), 1, file);
    fread(&fih.biXPelsPerMeter, sizeof(fih.biXPelsPerMeter), 1, file);
    fread(&fih.biYPelsPerMeter, sizeof(fih.biYPelsPerMeter), 1, file);
    fread(&fih.biClrUsed, sizeof(fih.biClrUsed), 1, file);
    fread(&fih.biClrImportant, sizeof(fih.biClrImportant), 1, file);

    int isize = fih.biSizeImage;
    int isizehalf = isize / 2;
    clock_t start, stop, startf, stopf; // clocks

    BYTE *pix = (BYTE *)mmap(NULL, isize, PROT_READ | PROT_WRITE, // Making space for pic data
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    fread(pix, isize, 1, file); // Read in file and close

    fclose(file);

    if (isizehalf % 3 != 0) // To compensate for halving landing in the middle of the bgr values
    {
        if (isizehalf % 3 == 1)
        {
            isizehalf -= 1;
        }
        else
        {
            isizehalf -= 2;
        }
    }

    startf = clock(); // start fork

    // if (fork() == 0) // child
    // {
    //     colorGrading(pix, isizehalf, b_grade, g_grade, r_grade);
    //     return 0;
    // }
    // else // parent
    // {
    //     wait(0);
    //     colorGrading(pix + isizehalf, isizehalf, b_grade, g_grade, r_grade);
    // }

    stopf = clock(); // end fork

    start = clock(); // start normal

    // colorGrading(pix, isize, b_grade, g_grade, r_grade);

    //Need to figure out why the flowers picture is coming out all whack
    //Something to do with the rows vs columns?
    // Am I coloring the wrong pixels?

    int wid = fih.biWidth;
    int len = fih.biHeight;

    for (int i = 0; i < wid; i++)
    {

        float temp = (float)pix[i];
        temp = temp / 255;
        if (i % 3 == 0)
        {
            temp = temp * b_grade; // blue
        }
        else if (i % 3 == 1)
        {
            temp = temp * g_grade; // green
        }
        else if (i % 3 == 2)
        {
            temp = temp * r_grade; // red
        }

        temp = temp * 255;
        pix[i] = (BYTE)temp;
        // pix[i] = 0;
    }

    stop = clock(); // end normal
    outFile(fh, fih, pix, fileOut);
    munmap(pix, isize);

    // cout << "startf: " << startf << endl;
    // cout << "stopf: " << stopf << endl;
    // cout << "start: " << start << endl;
    // cout << "stop: " << stop << endl;
    cout << "Fork: " << stopf - startf << endl;
    cout << "Non-Fork: " << stop - start << endl;
    cout << "Difference: " << (stop - start) - (stopf - startf) << endl;

    return 0;
}

void colorGrading(BYTE *pix, int size, float b_grade, float g_grade, float r_grade)
{

    for (int i = 0; i < size; i++)
    {
        float temp = (float)pix[i];
        temp = temp / 255;
        if (i % 3 == 0)
        {
            temp = temp * b_grade; // blue
        }
        else if (i % 3 == 1)
        {
            temp = temp * g_grade; // green
        }
        else if (i % 3 == 2)
        {
            temp = temp * r_grade; // red
        }

        temp = temp * 255;
        pix[i] = (BYTE)temp;
    }
}

void outFile(tagBIGMAPFILEHEADER fh, tagBITMAPINFOHEADER fih, BYTE *pix, char *fileOut)
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
    fwrite(pix, isize, 1, outFile);
    fclose(outFile);
}
