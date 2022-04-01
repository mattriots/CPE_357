#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

class testc
{
public:
    float f;
    int i;

    testc()
    {
        f = 1.6;
        i = 12345;
    }
};

typedef struct mystruct{

int data; float f;

} mystructure;


mystructure *n = (mystructure*)malloc(sizeof(mystructure));
(*n).data = 9;
(*n).data = 9;

mystruc *head = NULL;
head->data = 1;

mystruc *n =(mystruc*)malloc(sizeof(mystruc));
(*n).data = 9;





typedef unsigned char BYTE;

int main()
{
    testc a;

    int o = 8;
    float *fp = (float*)&o;
    float j = *fp;

    cout << j << endl;

    BYTE *p = (BYTE *)&a;

    for(int i = 0; i < 8; i ++){
         cout << (int)p[i] << endl;
    }



    return 0;
}
