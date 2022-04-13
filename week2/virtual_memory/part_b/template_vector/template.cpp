#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;

typedef unsigned char BYTE;

template <class A> // write template <class name>
                   // A will now be the datatype.
class lle
{
public:
    A data; // A data type can be converted to whatever later in the code

    lle *prev, *next;
    lle()
    {
        prev, next = NULL;
    }
    //.....
};

int main()
{
    ///------------------------------------------------------

    // lle<int> a;  //call it here with the <datatype>
    //             // ex. lle<int>. Now data is int

    // a.data = 8;

    // lle<string> b;
    // b.data = "hello";

    ///------------------------------------------------------

    // vector makes a dynamic array
    vector<int> numbers; // int numbers[]. basically an array of ints
    numbers.push_back(5);
    numbers.push_back(4);
    numbers.push_back(7);

    for (int i = 0; i < numbers.size(); i++)
    {
        cout << numbers[i] << endl;
    }

    numbers.push_back(99);

    for (int i = 0; i < numbers.size(); i++)
    {
        cout << numbers[i] << endl;
    }

    numbers.resize(10); // starts with size 10;

    vector<string> hello; // int numbers[]. basically an array of ints
    hello.push_back("hello");
    hello.push_back(" suck");
    hello.push_back(" it");

    for (int i = 0; i < hello.size(); i++)
    {
        cout << hello[i] <<endl;
    }



    return 0;
}
