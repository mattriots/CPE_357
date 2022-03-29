#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;


void fct(int &y) { //int &y <-- passing by ref for C++
   y = 9;
}

int main() {
  int u = 10;

  int* p;   // declare pointer
  
  p = &u;   // asign pointer     // This is all in C
  p = NULL; // can assign pointer with NULL
  
  if(p == 0){
      cout << "error" << endl;
      return 0;
  }

//   *p = 11;  // * is dereference operator
                //manipulates original variable aka 'u'

  fct(u);

  cout << u << endl;
  return 0;
}