#include <stdio.h>
#include <iostream>

void fct(int k[100]) {
  k[0] = 7;
}

typedef struct vec3d {
  float x, y, z;
  int j;
  char text[100];
} vec3d;

class cvec3d {
 private:
 public:
  float x, y, z;
  void printit() 
  {
    printf("x=%f\n", x);
    printf("y=%f\n", y);
    printf("z=%f\n", z);
  }
  cvec3d() { x = y = z = 0.0; }
  ~cvec3d() { printf("byebye\n"); }
};

using namespace std;

int main() {
  cvec3d r;
  r.x = 9.6;
  r.printit();
  r.x = 8;
  r.y = 5.5;

  double h;
  float g;
  g = 9.5;
  int arr[100];  // 0 - 99
  arr[100] = 8;

  fct(arr);
  printf("our arr var = %d \n", arr[0]);

  cout << "lets print stuff " << arr[0] << "\n";
  cin >> arr[0];

  return 0;
}
