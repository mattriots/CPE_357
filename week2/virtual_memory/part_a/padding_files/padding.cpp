#include <stdio.h>
#include <strings.h>
#include <iostream>
#include <fstream> //for reading files in C++

using namespace std;


typedef unsigned char BYTE;
typedef struct mystruct
{
    BYTE b;
    int i;
} mystruct;

int main()
{
    //C++ Notation
    ifstream file("test.bin"); //opens file
    // file.open("test.bin"); //another way to do it
    
    if(!file.is_open()){  //check to see if file exists or is open?
        return 0;       //if its not, exit program
    }
    mystruct s;

    //read in FROM file
    file >> s.b;
    file >> s.i;

    file.close();

    //write TO file
    ofstream ofile("out.bin");
    ofile << s.b;
    ofile << s.i;

    ofile.close();





    //C Notation
    // read bytefile in
    FILE *file = fopen("test.bin", "rb"); //rb = read byte
    mystruct s; //instace of struct
        //fread(reading into, size, how many times, from where)
    fread(&s.b, sizeof(s.b), 1, file);
    fread(&s.i, sizeof(s.i), 1, file); //have to read them in individually
                                        //or padding will throw us off

    fclose(file);
    cout << (int)s.b << endl;
    cout << s.i << endl;

    // Write bytefile out
     FILE *file = fopen("test.bin", "wb"); //open a file, write binary
                    // fopen("file", "what to do with file")
                    // w = write, r = read, wb = write binary, rb = read binary

     BYTE y = 5;
     int i = 99;
     cout << &i << endl;
        //fwrite(address of what to write, how much, how many times, to file pointer)
     fwrite(&y, 1, 1, file);
     fwrite(&i, 4, 1, file);


     fclose(file);   
    // close file

    return 0;
}