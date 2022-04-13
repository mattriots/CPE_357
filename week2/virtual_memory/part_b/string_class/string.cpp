#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

using namespace std;

typedef unsigned char BYTE;


// class string{               //pseudo code of what string class
//     private:                // might look like
//      char *text;

//     public:
//     char *c_str(){
//         return text;}

// };

int main(){

    char text[1000];
    strcpy(text,"hello");

    string a;
    a = text; // converts char array to string
    a = "hello world "; // sets letters to string
    a += "my name is Mart"; // contats string to other string
    string b = a.substr(3); //substr(start, length)


    strcpy(text,b.c_str()); //converts a string back into char array
                            //strcpy(destination char array, string)


    cout << a << endl;
    cout << b << endl;
    cout << text << endl;



    return 0;
}