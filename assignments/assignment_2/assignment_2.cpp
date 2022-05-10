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

void inFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, FILE *fileIn);
void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *pix, char *fileOut);
void blend(BYTE *pixbig, BYTE *pixsmall, BYTE *dataStore, int width, int height, float ratio);

int main(int argc, char **argv)
{

    ////////////////////////////////////
    // VARIABLES, SETUP, CLI INPUT    //
    ///////////////////////////////////


    // char *fileInNameBig = argv[1];
    // char *fileInNameSmall = argv[2];
    // float ratio = stof(argv[3]);
    // char *fileOut = argv[4];

    char *fileInNameBig = "lion.bmp";
    char *fileInNameSmall = "wolf.bmp";
    float ratio = .5;
    char *fileOut = "image.bmp";


    tagBIGMAPFILEHEADER fhbig; // Struct var for big fileheaders
    tagBITMAPINFOHEADER fihbig;

    tagBIGMAPFILEHEADER fhsmall; // Struct var for small fileheaders
    tagBITMAPINFOHEADER fihsmall;

    FILE *fileInBig = fopen(fileInNameBig, "rb"); // Open Big pic
    inFile(fhbig, fihbig, fileInBig);          // Read in all the header data from file

    FILE *fileInSmall = fopen(fileInNameSmall, "rb"); // Open small pic
    inFile(fhsmall, fihsmall, fileInSmall);      // Read in all the header data from file

    ///// local variables //Bigger picture
    int isizebig = fihbig.biSizeImage; // size of image
    int widthbig = fihbig.biWidth * 3; // Width in bytes
    int heightbig = fihbig.biHeight;   // Height in pixels

    ///// local variables //Smaller picture
    int isizesmall = fihsmall.biSizeImage; // size of image
    int widthsmall = fihsmall.biWidth * 3; // Width in bytes
    int heightsmall = fihsmall.biHeight;   // Height in pixels

    // Need to make this the size of the bigger picture
    //But making two seperate ones for now
    BYTE *datastorebig = (BYTE *)malloc(widthbig * heightbig * 3); // making space for data storage
                                                                //  This is where the altered data will live

    BYTE *datastoresmall = (BYTE *)malloc(widthsmall * heightsmall * 3); // making space for data storage
    //                                                             //  This is where the altered data will live

    BYTE *pixbig = (BYTE *)mmap(NULL, isizebig, PROT_READ | PROT_WRITE, // Making space for pic data
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    BYTE *pixsmall = (BYTE *)mmap(NULL, isizesmall, PROT_READ | PROT_WRITE, // Making space for pic data
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    fread(pixbig, isizebig, 1, fileInBig);       // Read in pix data
    fread(pixsmall, isizesmall, 1, fileInSmall); // Read in pix data

    fclose(fileInBig);   // Close file (especially before fork)
    fclose(fileInSmall); // Close file (especially before fork)

    blend(pixbig, pixsmall, datastorebig, widthbig, heightbig, ratio);

    outFile(fhbig, fihbig, datastorebig, "result.bmp"); // Write file out
    // outFile(fhsmall, fihsmall, pixsmall, "smalldic.bmp"); // Write file out

    munmap(pixbig, isizebig);
    munmap(pixsmall, isizesmall);
    free(datastorebig);
    free(datastoresmall);

}

void blend(BYTE *pixbig, BYTE *pixsmall, BYTE *dataStore, int width, int height, float ratio)
{

    // Ceiling function to catch padding problems
    if ((width % 4 != 0))
    {
        width += 4 - width % 4;
    }

    for (int j = 0; j < height; j++) // height or # of rows
    {

        for (int i = 0; i < width; i++) // width or bytes per row
        {
            BYTE b_big, g_big, r_big, b_small, g_small, r_small; // local BYTE variables to hold pix data

            b_big = pixbig[(width * j) + i * 3 + 0];
            g_big = pixbig[(width * j) + i * 3 + 1];
            r_big = pixbig[(width * j) + i * 3 + 2];

            b_small = pixsmall[(width * j) + i * 3 + 0];
            g_small = pixsmall[(width * j) + i * 3 + 1];
            r_small = pixsmall[(width * j) + i * 3 + 2];

            dataStore[(width * j) + i * 3 + 0] = b_big * ratio + b_small * (1-ratio);
            dataStore[(width * j) + i * 3 + 1] = g_big * ratio + g_small * (1-ratio);
            dataStore[(width * j) + i * 3 + 2] = r_big * ratio + r_small * (1-ratio);
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