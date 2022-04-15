#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>

#include <sys/time.h>
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
void colorGrading(BYTE *pix, BYTE *dataStore, int width, int hiStart, int hiEnd, float b_grade, float g_grade, float r_grade);

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

    tagBIGMAPFILEHEADER fh; // Struct var for fileheaders
    tagBITMAPINFOHEADER fih;

    struct timeval start, end, startf, endf; //Struct vars for timing

    FILE *fileIn = fopen(fileInName, "rb"); // Open file

    inFile(fh, fih, fileIn); // Read in all the header data from file

    ///// local variables
    int isize = fih.biSizeImage; // size of image
    int width = fih.biWidth * 3; // Width in bytes
    int height = fih.biHeight;   // Height in pixels

    ////////////////////////////
    // MAKING SPACE          //
    ///////////////////////////

    BYTE *dataStore = (BYTE *)malloc(width*height*3); // making space for data storage
                                            //  This is where the altered data will live

    BYTE *pix = (BYTE *)mmap(NULL, isize, PROT_READ | PROT_WRITE, // Making space for pic data
                             MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    fread(pix, isize, 1, fileIn); // Read in pix data

    fclose(fileIn); // Close file (especially before fork)

    ////////////////////
    // FORK TIME      //
    ////////////////////


    gettimeofday(&startf, NULL);  // start timing fork -- using time of day

    int pid = fork();

    if (pid == 0) // child
    {
        colorGrading(pix, dataStore, width, 0, height / 2, b_grade, g_grade, r_grade);

        return 0;
    }
    else
    {
        // parent
        colorGrading(pix, dataStore, width, height / 2, height, b_grade, g_grade, r_grade);

        wait(NULL); // Wait AFTER the parent does its work
    }

    gettimeofday(&endf, NULL);  //end timing fork -- using time of day

    long durationF = 1000000 * (endf.tv_sec - startf.tv_sec) + (endf.tv_usec - startf.tv_usec); //Duration of for in microseconds


    ////////////////////////////////////
    // NORMAL FUNCTION TIME    //
    ///////////////////////////////////


    gettimeofday(&start, NULL);  // start timing normal -- using time of day


    colorGrading(pix, dataStore, width, 0, height, b_grade, g_grade, r_grade);


    gettimeofday(&end, NULL);  // end timing normal -- using time of day

    long duration = 1000000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec);


    ////////////////////////////////////
    // PRINTOUT AND CLOSE   //
    ///////////////////////////////////

    outFile(fh, fih, dataStore, fileOut); // Write file out

    munmap(pix, isize); // Clear mmap
    free(dataStore);    // Free up malloc
    
    cout << "Fork: " << durationF << endl;
    cout << "Non-Fork: " << duration << endl;
    cout << "Difference: " << duration - durationF << endl;

    return 0;
}

void colorGrading(BYTE *pix, BYTE *dataStore, int width, int hiStart, int hiEnd, float b_grade, float g_grade, float r_grade)
{

    // Ceiling function to catch padding problems
    if ((width % 4 != 0))
    {
        width += 4 - width % 4;
    }

    for (int j = hiStart; j < hiEnd; j++) // height or # of rows
    {

        for (int i = 0; i < width; i++) // width or bytes per row
        {
            BYTE b, g, r; // local BYTE variables to hold pix data

            b = pix[(width * j) + i * 3 + 0];
            g = pix[(width * j) + i * 3 + 1];
            r = pix[(width * j) + i * 3 + 2];

            dataStore[(width * j) + i * 3 + 0] = b * b_grade; /// Store pix * grade --> data store
            dataStore[(width * j) + i * 3 + 1] = g * g_grade;
            dataStore[(width * j) + i * 3 + 2] = r * r_grade;
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
