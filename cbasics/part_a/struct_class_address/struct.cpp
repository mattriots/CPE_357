#include <string.h>
#include <iostream>

using namespace std;

class testc {  // class size is the size of the variables only
               // in this case 8 bytes
 public:
  int i;
  float f;
  testc() {   //construct
    i = 12345;
    f = 1.6f;
  }
};

typedef unsigned char BYTE; //makes an unsigned char type called BYTE

int main() {
  testc a;
  BYTE *p = (BYTE*)&a;  //address of class a

  int *ip = (int *)p; //a.i
  float *fp = (float *)(p+4); //a.f

  cout << &a << endl;  //address of the class is same as address
  cout << &a.i << endl;// of first element
  cout << &a.f << endl;

  cout << (int)p[0] << endl;
  cout << (int)p[1] << endl;  //Printing all 4 bytes of the integer i
  cout << (int)p[2] << endl;  // 12345
  cout << (int)p[3] << endl;

  cout<< *ip << endl;
  cout<< *fp << endl;


  // char text[] = "hello folks";

  // strcat(text, "hi");

  int a;
  int *q = &a;
  p = 1;
  *p = 13;

  return 0;
}