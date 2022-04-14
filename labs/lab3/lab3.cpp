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

void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *pix, char *fileOut);
void inFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, FILE *fileIn);
void colorGrading(BYTE *pix, int width, int hiStart, int hiEnd, float b_grade, float g_grade, float r_grade);

int main(int argc, char **argv)
{
    ////////////////////////////////////
    // VARIABLES, SETUP, CLI INPUT    //
    ///////////////////////////////////
    char *fileInName = argv[1];
    float b_grade = stof(argv[2]);
    float g_grade = stof(argv[3]);
    float r_grade = stof(argv[4]);
    char *fileOut = argv[5];

    tagBIGMAPFILEHEADER fh;  // Structs for fileheaders
    tagBITMAPINFOHEADER fih; 

    clock_t start, stop, startf, stopf; // clocks

    FILE *fileIn = fopen(fileInName, "rb"); // Open file

    inFile(fh, fih, fileIn); //Read in all the header data from file

    ///// local variables
    int isize = fih.biSizeImage; //size of image
    int width = fih.biWidth * 3; // Width in bytes
    int height = fih.biHeight;   // Height in pixels

     ////////////////////////////
    // MAKING SPACE 1         //
    ///////////////////////////

    BYTE *pix = (BYTE *)mmap(NULL, isize, PROT_READ | PROT_WRITE, // Making space for pic data
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    fread(pix, isize, 1, fileIn); // Read in pix data

    fclose(fileIn); // Close

    ////////////////////
    // FORK TIME      //
    ////////////////////

    startf = clock(); // start fork

    if (fork() == 0) // child
    {
        colorGrading(pix, width, 0, height / 2, b_grade, g_grade, r_grade);
        return 0;
    }
    else // parent
    {
        wait(0);
        colorGrading(pix, width, height / 2, height, b_grade, g_grade, r_grade);
    }

    stopf = clock(); // end fork

    munmap(pix, isize); // Clear space


    //////////////////////////////////////
    // MAKING SPACE 2 & RE-READ IN FILE //
    //////////////////////////////////////

    fileIn = fopen(fileInName, "rb"); // Open file

    pix = (BYTE *)mmap(NULL, isize, PROT_READ | PROT_WRITE,     // Making space for pic data
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    inFile(fh, fih, fileIn);

    fread(pix, isize, 1, fileIn); // Read in pix data

    fclose(fileIn); // Close


    ////////////////////////////////////
    // NORMAL FUNCTION TIME    //
    ///////////////////////////////////

    start = clock(); // start normal

    colorGrading(pix, width, 0, height, b_grade, g_grade, r_grade);

    stop = clock(); // end normal


    ////////////////////////////////////
    // PRINTOUT AND CLOSE   //
    ///////////////////////////////////

    outFile(fh, fih, pix, fileOut);  //Write file out

    munmap(pix, isize);  //Clean up space

    cout << "Fork: " << stopf - startf << endl;
    cout << "Non-Fork: " << stop - start << endl;
    cout << "Difference: " << (stop - start) - (stopf - startf) << endl;

    return 0;
}

void colorGrading(BYTE *pix, int width, int hiStart, int hiEnd, float b_grade, float g_grade, float r_grade)
{

    // Ceiling function to catch padding problems
    if ((width % 4 != 0))
    {
        if (width % 4 == 1)
        {
            width += 3;
        }
        else if (width % 4 == 2)
        {
            width += 2;
        }
        else if (width % 4 == 3)
        {
            width += 1;
        }
    }

    for (int j = hiStart; j < hiEnd; j++) // height or # of rows
    {

        for (int i = 0; i < width; i++) // width or bytes per row
        {

            // float temp = (float)pix[(width * j) + i];
            // BYTE tempB = pix[(width * j) + i];
            // temp = temp / 255;
            if (i % 3 == 0)
            {
                // temp = temp * b_grade; // blue
                pix[(width * j) + i] *= b_grade; //blue
            }
            else if (i % 3 == 1)
            {
                // temp = temp * g_grade; // green
                pix[(width * j) + i] *= g_grade;  //green
            }
            else if (i % 3 == 2)
            {
                // temp = temp * r_grade; // red
                pix[(width * j) + i] *= r_grade;  //red
            }

            // temp = temp * 255;
            // pix[((width * j) + i)] = (BYTE)temp;
        }
    }
}
// passing by reference with &  <----- REMEMBER THIS
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

void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *pix, char *fileOut)
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
