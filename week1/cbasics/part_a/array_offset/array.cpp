#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;
                    //   *a is accepting a pointer
                    // and since arr is a pointer it works
//could also do void fct(int *a)
void fct(int a[100]){
    a[5] =7;
}


int main(){
    int arr[100]; //array elements are created as neighbors in mem
                    //arr[0] is a pointer to address of first element
                    //arr = &arr[0]
    for(int i =0; i< 100; i++){
        arr[i]=0;
    }

    int *p = arr;

    //p[5] = 7; //this alters 5th element of arr
    //*(p+5)   //this is the smae as the above line
    // p = p + 5; // this is the same as above line

    *p = 8; 

    // void *k = p;
    unsigned char *c = (unsigned char*)p;

    c = c + 5; // this is the same as above line

    *c = 8; 


    // fct(arr);

    
    cout << arr[5]<< endl;
}