#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

// float, int <--32 bit, 4 byte
// char <--- 8 bit, 1 byte

class vec3d {  //12 bytes
 public:
  float x, y, z;
};


class myclass { // 8 bytes
 public:        // padding - pads the unused bytes from the char
                // so that everything is in groups of 4
                // since every cell is in 4 byte groups
  char c;
  int i;
};

int main() {
  // float f = 9;
  // float *y;
  // y = &f;

  int g;     // 4 byte
  float f;   // 4 byte
  char c;    // 1 byte
  double d;  // 8 byte

  char text[100];  // 100 byte
  vec3d v; // 12 byte cuz 3 floats in class
  myclass my; // 8 bytes


  int i = sizeof(g);
  int j = sizeof(f);
  int k = sizeof(c);
  int l = sizeof(d);
  int m = sizeof(text);
  int n = sizeof(v);
  int o = sizeof(my);

  cout << i << endl;
  cout << j << endl;
  cout << k << endl;
  cout << l << endl;
  cout << m << endl;
  cout << n << endl;
  cout << o << endl;
  // unsigned int h;

  return 0;
}