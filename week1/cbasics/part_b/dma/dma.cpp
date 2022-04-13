#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;

class students
{
public:
   char name[100];
   char major[100];

    students()
    {
    }
};

typedef unsigned char BYTE;

int main()
{
    // int a[100];
    students a;

    students *p = NULL;

    students *r = new students;
    *r.major = a.major;




    p = (students*) malloc(200);  //malloc(bytes) provides you more memory, dynamically
                                    //in bytes
    p = (students*) malloc(sizeof(students) * 10); //C way of making space for
                                                //array size 10

    p = new students; // C++ version;
    p = new students[10]; // C++ version;

    if(p==NULL){
        cout << "out of mem"<<endl;  //check to see if p is still null
        return 0;                       //after malloc() operation
    }

    cin >> (*p).major;  //have to dereference p since p is a pointer
    cin >> p->major;    //exactly the same as above
    cout << p->major << endl; //before using it as an instance

    delete[] p; // c++ delete whole array
    delete(p); // c++
    free(p);  // c

    return 0;
}



/*
Your program will have a spot reserved right
off the bat.
Then OS will show you where there is more memory.


*/ 

