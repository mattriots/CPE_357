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

void inFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, FILE *fileIn);
void outFile(tagBIGMAPFILEHEADER &fh, tagBITMAPINFOHEADER &fih, BYTE *pix, char *fileOut);
void multiply(float *pix1, float *pix2, float *datastore, int width1, int height1, int width2, int height2);
void normalize(BYTE *pix, float *pixStore, int size, int width, int height);
void finalize(float *datastore, BYTE *resultstore, int size);
void quadratic_matrix_print(float *C, int width1, int height1);

int main(int argc, char **argv)
{

    // MAKE NAMED SHARED MEMORY FOR
    //  pix1, pix2, resultimage, ready

    ////////////////////////////////////
    // VARIABLES, SETUP, CLI INPUT    //
    ///////////////////////////////////

    // char *fileInName1 = argv[1];
    // char *fileInName2 = argv[2];
    // float ratio = stof(argv[3]);
    // char *fileOut = argv[4];

    char *fileInName1 = "f1.bmp";
    char *fileInName2 = "f2.bmp";
    // float ratio = stof(argv[3]);
    char *fileOut = "result.bmp";

    tagBIGMAPFILEHEADER fh1; // Struct var for first fileheaders
    tagBITMAPINFOHEADER fih1;

    tagBIGMAPFILEHEADER fh2; // Struct var for second fileheaders
    tagBITMAPINFOHEADER fih2;

    FILE *fileIn1 = fopen(fileInName1, "rb"); // Open first pic
    inFile(fh1, fih1, fileIn1);               // Read in all the header data from file

    FILE *fileIn2 = fopen(fileInName2, "rb"); // Open small pic
    inFile(fh2, fih2, fileIn2);               // Read in all the header data from file

    int isize1, width1, height1, isize2, width2, height2;

    // Assign size | width | height

    isize1 = fih1.biSizeImage;
    width1 = fih1.biWidth * 3;
    height1 = fih1.biHeight;

    isize2 = fih2.biSizeImage;
    width2 = fih2.biWidth * 3;
    height2 = fih2.biHeight;

    // Need to make this the size of the result

    float *pix1Store = (float *)malloc(width1 * height1 * sizeof(float));
    float *pix2Store = (float *)malloc(width1 * height1 * sizeof(float));
    float *datastore = (float *)malloc(width1 * height1 * sizeof(float));
    BYTE *resultstore = (BYTE *)malloc(width1 * height1 * 3);

    // Space for the 2 picture pixels
    BYTE *pix1 = (BYTE *)malloc(isize1);
    BYTE *pix2 = (BYTE *)malloc(isize2);

    fread(pix1, isize1, 1, fileIn1); // Read in pix data
    fread(pix2, isize2, 1, fileIn2); // Read in pix data

    fclose(fileIn1); // Close file
    fclose(fileIn2); // Close file

    normalize(pix1, pix1Store, isize1, width1, height1);
    normalize(pix2, pix2Store, isize2, width2, height2);

    multiply(pix1Store, pix2Store, datastore, width1, height1, width2, height2);

    quadratic_matrix_print(datastore, width1, height1);

    finalize(datastore, resultstore, isize1);

    outFile(fh1, fih1, resultstore, fileOut); // Write file out

    free(datastore);
    free(pix1Store);
    free(pix2Store);
    free(pix1);
    free(pix2);

    return 0;
}

void normalize(BYTE *pix, float *pixStore, int size, int width, int height)
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
        float temp = (float)datastore[i];
        temp = temp * 255;
        temp = temp * 0.03;
        resultstore[i] = (BYTE)temp;
    }
}

void quadratic_matrix_print(float *C, int width1, int height1)
{
    int count = 0;
    printf("\n");
    for (int a = 0; a < width1; a++)
    {
        printf("\n");
        for (int b = 0; b < height1; b++)
        {
            printf("%.2f,", C[a + b * width1]);
            count++;
        }
    }
    printf("\ncount: %d\n", count);
}

void multiply(float *pix1, float *pix2, float *datastore, int width1, int height1, int width2, int height2)
{

    int par_id = 1;
    int par_count = 1;

    for (int a = 0; a < 300; a++)
        for (int b = 0; b < 300 * 3; b++)
            datastore[a + b * 300] = 0.0;
    // multiply
    for (int a = 0; a < 300; a++)         // over all cols a
        for (int b = 0; b < width1; b++)  // over all rows b
            for (int c = 0; c < 300; c++) // over all rows/cols left
            {
                // datastore[a + b * 300] += pix1[c + b * 300] * pix2[a + c * 300];
                // int datanum = a + b * 300;
                // int pix1num = c + b * 300;
                // int pix2num = a + c * 300;
                // int x = 0;
                datastore[(width1 * a) + b * 3 + 0] += pix1[(300 * c) + b * 3 + 0] * pix2[(width1 * a) + c * 3 + 0];
                datastore[(width1 * a) + b * 3 + 1] += pix1[(300 * c) + b * 3 + 1] * pix2[(width1 * a) + c * 3 + 1];
                datastore[(width1 * a) + b * 3 + 2] += pix1[(300 * c) + b * 3 + 2] * pix2[(width1 * a) + c * 3 + 2];

                // datastore[a + b * 300 + 1] += pix1[c + b * 300 + 1] * pix2[a + c * 300 + 1];
                // int datanum1 = a + b * 300 + 1;
                // int pix1num1 = c + b * 300 + 1;
                // int pix2num1 = a + c * 300 + 1;
                // int x = 0;
                // datastore[a + b * 300 + 2] += pix1[c + b * 300 + 2] * pix2[a + c * 300 + 2];
                // int datanum2 = a + b * 300 + 2;
                // int pix1num2 = c + b * 300 + 2;
                // int pix2num2 = a + c * 300 + 2;
                // x = 0;
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