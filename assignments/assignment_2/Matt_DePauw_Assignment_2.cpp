#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>

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
void manPage(int argc);
void inFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, FILE *fileIn);
void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *pix, char *fileOut);
void blend(BYTE *pixbig, BYTE *pixsmall, BYTE *dataStore, int width1, int height1, int width2, int height2, float ratio);
BYTE get_red(BYTE *pix, float x, float y, int width, int height);

int main(int argc, char **argv)
{

    ////////////////////////////////////
    // VARIABLES, SETUP, CLI INPUT    //
    ///////////////////////////////////
    int param = argc;

    if (argc != 5)
    {
        manPage(argc);
    }
    else
    {

        char *fileInName1 = argv[1];
        char *fileInName2 = argv[2];
        float ratio = stof(argv[3]);
        char *fileOut = argv[4];

        tagBIGMAPFILEHEADER fh1; // Struct var for first fileheaders
        tagBITMAPINFOHEADER fih1;

        tagBIGMAPFILEHEADER fh2; // Struct var for second fileheaders
        tagBITMAPINFOHEADER fih2;

        FILE *fileIn1 = fopen(fileInName1, "rb"); // Open first pic
        inFile(fh1, fih1, fileIn1);               // Read in all the header data from file

        FILE *fileIn2 = fopen(fileInName2, "rb"); // Open small pic
        inFile(fh2, fih2, fileIn2);               // Read in all the header data from file

        int isize1, width1, height1, isize2, width2, height2, widthbig, heightbig;
        tagBIGMAPFILEHEADER fhbig;
        tagBITMAPINFOHEADER fihbig;

        isize1 = fih1.biSizeImage;
        width1 = fih1.biWidth * 3;
        height1 = fih1.biHeight;

        isize2 = fih2.biSizeImage;
        width2 = fih2.biWidth * 3;
        height2 = fih2.biHeight;

        if (width1 >= width2)
        {

            widthbig = width1;   // Width in bytes
            heightbig = height1; // Height in pixels
            fhbig = fh1;
            fihbig = fih1;
        }
        else
        {
            widthbig = width2;   // Width in bytes
            heightbig = height2; // Height in pixels
            fhbig = fh2;
            fihbig = fih2;
        }

        // Need to make this the size of the bigger picture
        BYTE *datastorebig = (BYTE *)malloc(widthbig * heightbig * 3);

        // Space for the 2 picture pixels
        BYTE *pix1 = (BYTE *)malloc(isize1);
        BYTE *pix2 = (BYTE *)malloc(isize2);

        fread(pix1, isize1, 1, fileIn1); // Read in pix data
        fread(pix2, isize2, 1, fileIn2); // Read in pix data

        fclose(fileIn1); // Close file
        fclose(fileIn2); // Close file

        blend(pix1, pix2, datastorebig, width1, height1, width2, height2, ratio);

        outFile(fhbig, fihbig, datastorebig, fileOut); // Write file out

        free(datastorebig);
        free(pix1);
        free(pix2);

        return 0;
    }
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

    leftup = pix[(width * y_up) + x_left * 3 + 0];
    leftdown = pix[(width * y_down) + x_left * 3 + 0];
    rightup = pix[(width * y_up) + x_right * 3 + 0];
    rightdown = pix[(width * y_down) + x_right * 3 + 0];

    left = leftup * dy + leftdown * (1 - dy);
    right = rightup * dy + rightdown * (1 - dy);
    result = left * (1 - dx) + right * dx;

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

    leftup = pix[(width * y_up) + x_left * 3 + 1];
    leftdown = pix[(width * y_down) + x_left * 3 + 1];
    rightup = pix[(width * y_up) + x_right * 3 + 1];
    rightdown = pix[(width * y_down) + x_right * 3 + 1];

    left = leftup * dy + leftdown * (1 - dy);
    right = rightup * dy + rightdown * (1 - dy);
    result = left * (1 - dx) + right * dx;
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

    left = leftup * dy + leftdown * (1 - dy);
    right = rightup * dy + rightdown * (1 - dy);
    result = left * (1 - dx) + right * dx;

    return result;
}

void blend(BYTE *pix1, BYTE *pix2, BYTE *dataStore, int width1, int height1, int width2, int height2, float ratio)
{
    int widthbig, heightbig, widthsmall, heightsmall, ratioselect;
    BYTE *pixbig, *pixsmall;

    if (width1 >= width2)
    {
        widthbig = width1;
        heightbig = height1;
        widthsmall = width2;
        heightsmall = height2;
        pixbig = pix1;
        pixsmall = pix2;
        ratioselect = 1;
    }
    else
    {
        widthbig = width2;
        heightbig = height2;
        widthsmall = width1;
        heightsmall = height1;
        pixbig = pix2;
        pixsmall = pix1;
        ratioselect = 2;
    }

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
            BYTE b_big, g_big, r_big, b_small, g_small, r_small;

            b_big = pixbig[(widthbig * j) + i * 3 + 0];
            g_big = pixbig[(widthbig * j) + i * 3 + 1];
            r_big = pixbig[(widthbig * j) + i * 3 + 2];

            x = i * widthratio;
            y = j * heightratio;

            b_small = get_blue(pixsmall, x, y, widthsmall, heightsmall);
            g_small = get_green(pixsmall, x, y, widthsmall, heightsmall);
            r_small = get_red(pixsmall, x, y, widthsmall, heightsmall);

            if (ratioselect == 1)
            {
                dataStore[(widthbig * j) + i * 3 + 0] = b_big * ratio + b_small * (1 - ratio);
                dataStore[(widthbig * j) + i * 3 + 1] = g_big * ratio + g_small * (1 - ratio);
                dataStore[(widthbig * j) + i * 3 + 2] = r_big * ratio + r_small * (1 - ratio);
            }
            else
            {

                dataStore[(widthbig * j) + i * 3 + 0] = b_big * (1 - ratio) + b_small * ratio;
                dataStore[(widthbig * j) + i * 3 + 1] = g_big * (1 - ratio) + g_small * ratio;
                dataStore[(widthbig * j) + i * 3 + 2] = r_big * (1 - ratio) + r_small * ratio;
            }
        }
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

void manPage(int argc)
{
    cout << "You entered " << argc - 1 << " argument(s)" << endl;
    cout << "You need to enter file name and 4 arguments" << endl;
    cout << "Usage: [program name] [imagefile1] [imagefile2] [ratio] [outfile]" << endl;
    cout << endl;
    cout << "Ex: ./assignment_2 jar.bmp lion.bmp 0.5 result.bmp" << endl;
    cout << endl;
    return;
}