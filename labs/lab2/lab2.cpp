#include <stdio.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cmath>

using namespace std;

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

// void contrast(BYTE &pix, int size);
// void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE &pix);

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

void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE &pix);
void contrast(BYTE &pix, int size);

int main()
{

    // create the file and read in all the header information
    FILE *file = fopen("jar.bmp", "rb");
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

    int isize = fih.biSizeImage; // size of image

    cout << "before : " << sbrk(0) << endl;

    BYTE *pix = (BYTE*)sbrk(isize);
    pix[isize];
    cout << "after  : " << sbrk(0) << endl;
    cout << "pix " << &pix << endl;

    fread(pix, isize, 1, file);

    fclose(file); // close the file like a good programmer

    contrast(*pix, isize); // Send the pixels off to be contrasted

    outFile(fh, fih, *pix); // Export the file

    return 0;
}

void contrast(BYTE &pix, int size)
{
    BYTE *p = &pix;
    float exponent = 3.0;

    cout << &p << endl;

    for (int i = 0; i < size; i++)
    {
        float temp = (float)p[i];
        temp = temp / 255;
        temp = pow(temp, exponent);
        temp = temp * 255;
        p[i] = (BYTE)temp;
    }
}

void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE &pix)
{

    FILE *outFile = fopen("out.bmp", "wb");

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
    fwrite(&pix, isize, 1, outFile);
}
