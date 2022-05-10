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
void blend(BYTE *pixbig, BYTE *pixsmall, BYTE *dataStore, int widthbig, int heightbig, int widthsmall, int heightsmall, float ratio);
BYTE get_red(BYTE *pix, float x, float y, int width, int height);

int main(int argc, char **argv)
{

    ////////////////////////////////////
    // VARIABLES, SETUP, CLI INPUT    //
    ///////////////////////////////////

    // char *fileInNameBig = argv[1];
    // char *fileInNameSmall = argv[2];
    // float ratio = stof(argv[3]);
    // char *fileOut = argv[4];

    char *fileInName1 = "lion.bmp";
    char *fileInName2 = "flowers.bmp";
    float ratio = 0.5;
    char *fileOut = "image.bmp";

    tagBIGMAPFILEHEADER fh1; // Struct var for big fileheaders
    tagBITMAPINFOHEADER fih1;

    tagBIGMAPFILEHEADER fh2; // Struct var for small fileheaders
    tagBITMAPINFOHEADER fih2;

    FILE *fileIn1 = fopen(fileInName1, "rb"); // Open Big pic
    inFile(fh1, fih1, fileIn1);               // Read in all the header data from file

    FILE *fileIn2 = fopen(fileInName2, "rb"); // Open small pic
    inFile(fh2, fih2, fileIn2);               // Read in all the header data from file

    ///// local variables //Bigger picture
    int isizebig = fih1.biSizeImage; // size of image
    int widthbig = fih1.biWidth * 3; // Width in bytes
    int heightbig = fih1.biHeight;   // Height in pixels

    ///// local variables //Smaller picture
    int isizesmall = fih2.biSizeImage; // size of image
    int widthsmall = fih2.biWidth * 3; // Width in bytes
    int heightsmall = fih2.biHeight;   // Height in pixels

    // Figure out which has biggest widgth
    // Guess had to fix all these varibales
    // If first width is bigger we change 1st to big, 2nd to small
    // Else change 2nd one to big and 1st to small

    // int isizebig, widthbig, heightbig, isizesmall, widthsmall, heightsmall;
    // FILE *fileInBig, *fileInSmall;
    tagBIGMAPFILEHEADER fhbig, fhsmall;
    tagBITMAPINFOHEADER fihbig, fihsmall;
    FILE *fileInBig;
    FILE *fileInSmall;

    if (fih1.biWidth >= fih2.biWidth)
    {
        ///// local variables //Bigger picture
        int isizebig = fih1.biSizeImage; // size of image
        int widthbig = fih1.biWidth * 3; // Width in bytes
        int heightbig = fih1.biHeight;   // Height in pixels
        fileInBig = fileIn1;
        fhbig = fh1;
        fihbig = fih1;

        ///// local variables //Smaller picture
        int isizesmall = fih2.biSizeImage; // size of image
        int widthsmall = fih2.biWidth * 3; // Width in bytes
        int heightsmall = fih2.biHeight;   // Height in pixels
        fileInSmall = fileIn2;
        fhsmall = fh2;
        fihsmall = fih2;
    }
    else
    {
        ///// local variables //Bigger picture
        int isizebig = fih2.biSizeImage; // size of image
        int widthbig = fih2.biWidth * 3; // Width in bytes
        int heightbig = fih2.biHeight;   // Height in pixels
        fileInBig = fileIn2;
        fhbig = fh2;
        fihbig = fih2;

        ///// local variables //Smaller picture
        int isizesmall = fih1.biSizeImage; // size of image
        int widthsmall = fih1.biWidth * 3; // Width in bytes
        int heightsmall = fih1.biHeight;   // Height in pixels
        fileInSmall = fileIn1;
        fhsmall = fh1;
        fihsmall = fih1;
    }

    // Need to make this the size of the bigger picture
    // But making two seperate ones for now
    BYTE *datastorebig = (BYTE *)malloc(widthbig * heightbig * 3); // making space for data storage
                                                                   //  This is where the altered data will live

    BYTE *datastoresmall = (BYTE *)malloc(widthsmall * heightsmall * 3); // making space for data storage

    BYTE *pixbig = (BYTE *)mmap(NULL, isizebig, PROT_READ | PROT_WRITE, // Making space for pic data
                                MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    BYTE *pixsmall = (BYTE *)mmap(NULL, isizesmall, PROT_READ | PROT_WRITE, // Making space for pic data
                                  MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    fread(pixbig, isizebig, 1, fileInBig);     // Read in pix data
    fread(pixsmall, isizesmall, 1, fileInSmall); // Read in pix data

    fclose(fileIn1); // Close file
    fclose(fileIn2); // Close file

    blend(pixbig, pixsmall, datastorebig, widthbig, heightbig, widthsmall, heightsmall, ratio);

    outFile(fhbig, fihbig, datastorebig, "result.bmp"); // Write file out
    // outFile(fhsmall, fihsmall, pixsmall, "smalldic.bmp"); // Write file out

    munmap(pixbig, isizebig);
    munmap(pixsmall, isizesmall);
    free(datastorebig);
    free(datastoresmall);
}

BYTE get_blue(BYTE *pix, float x, float y, int width, int height)
{
    int x_left = (int)x;
    int x_right = x_left + 1;
    int y_down = (int)y;
    int y_up = y_down + 1;
    int dx = x - x_left;
    int dy = y - y_down;
    BYTE leftup, rightup, leftdown, rightdown, left, right, result;

    leftup = pix[(width * y_up) + x_left * 3 + 2];
    leftdown = pix[(width * y_down) + x_left * 3 + 2];
    rightup = pix[(width * y_up) + x_right * 3 + 2];
    rightdown = pix[(width * y_down) + x_right * 3 + 2];

    left = leftup * (1 - dy) + leftdown * dy;
    right = rightup * (1 - dy) + rightdown * dy;
    result = left * dx + right * (1 - dx);

    return result;
}

BYTE get_green(BYTE *pix, float x, float y, int width, int height)
{
    int x_left = (int)x;
    int x_right = x_left + 1;
    int y_down = (int)y;
    int y_up = y_down + 1;
    int dx = x - x_left;
    int dy = y - y_down;
    BYTE leftup, rightup, leftdown, rightdown, left, right, result;

    leftup = pix[(width * y_up) + x_left * 3 + 2];
    leftdown = pix[(width * y_down) + x_left * 3 + 2];
    rightup = pix[(width * y_up) + x_right * 3 + 2];
    rightdown = pix[(width * y_down) + x_right * 3 + 2];

    left = leftup * (1 - dy) + leftdown * dy;
    right = rightup * (1 - dy) + rightdown * dy;
    result = left * dx + right * (1 - dx);

    return result;
}

BYTE get_red(BYTE *pix, float x, float y, int width, int height)
{
    int x_left = (int)x;
    int x_right = x_left + 1;
    int y_down = (int)y;
    int y_up = y_down + 1;
    int dx = x - x_left;
    int dy = y - y_down;
    BYTE leftup, rightup, leftdown, rightdown, left, right, result;

    leftup = pix[(width * y_up) + x_left * 3 + 2];
    leftdown = pix[(width * y_down) + x_left * 3 + 2];
    rightup = pix[(width * y_up) + x_right * 3 + 2];
    rightdown = pix[(width * y_down) + x_right * 3 + 2];

    left = leftup * (1 - dy) + leftdown * dy;
    right = rightup * (1 - dy) + rightdown * dy;
    result = left * dx + right * (1 - dx);

    return result;
}

void blend(BYTE *pixbig, BYTE *pixsmall, BYTE *dataStore, int widthbig, int heightbig, int widthsmall, int heightsmall, float ratio)
{

    float widthratio = (float)widthsmall / (float)widthbig;
    float heightratio = (float)heightsmall / (float)heightbig;
    float x;
    float y;

    // Ceiling function to catch padding problems
    if ((widthbig % 4 != 0))
    {
        widthbig += 4 - widthbig % 4;
    }
    if ((widthsmall % 4 != 0))
    {
        widthsmall += 4 - widthsmall % 4;
    }

    for (int j = 0; j < heightbig; j++) // height or # of rows
    {

        for (int i = 0; i < widthbig; i++) // width or bytes per row
        {
            BYTE b_big, g_big, r_big, b_small, g_small, r_small; // local BYTE variables to hold pix data

            b_big = pixbig[(widthbig * j) + i * 3 + 0];
            g_big = pixbig[(widthbig * j) + i * 3 + 1];
            r_big = pixbig[(widthbig * j) + i * 3 + 2];

            // b_small = pixsmall[(width * j) + i * 3 + 0];
            // g_small = pixsmall[(width * j) + i * 3 + 1];
            // r_small = pixsmall[(width * j) + i * 3 + 2];

            x = i * widthratio;
            y = j * heightratio;

            // b_small = pixsmall[(width * j) + i * 3 + 0];
            // g_small = pixsmall[(width * j) + i * 3 + 1];
            b_small = get_blue(pixsmall, x, y, widthsmall, heightsmall);
            g_small = get_green(pixsmall, x, y, widthsmall, heightsmall);
            r_small = get_red(pixsmall, x, y, widthsmall, heightsmall);

            dataStore[(widthbig * j) + i * 3 + 0] = b_big * ratio + b_small * (1 - ratio);
            dataStore[(widthbig * j) + i * 3 + 1] = g_big * ratio + g_small * (1 - ratio);
            dataStore[(widthbig * j) + i * 3 + 2] = r_big * ratio + r_small * (1 - ratio);
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