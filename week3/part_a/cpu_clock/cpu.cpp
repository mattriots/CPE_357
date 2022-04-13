#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#include <time.h>

using namespace std;

typedef unsigned char BYTE;

int main()
{

    int y = 0;

    for(... 1000.... ){   //Loop everything 1000 times and then
                        //Divide by 1000 to get average

    clock_t a = clock();
    // cout << a << endl;
    cout << (float)a / CLOCKS_PER_SEC << endl;

    for (int i = 0; i < 100000; i++)
    {
        y++;
    }

    a = clock();
    // cout << a << endl;
    cout << (float)a / CLOCKS_PER_SEC << endl;
    }
}