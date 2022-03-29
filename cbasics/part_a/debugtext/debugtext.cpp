#include <stdio.h>
#include <string.h>
#include <iostream>

using namespace std;

int main() {
  int i = 9;
  int u = 6;
  char text[60];

    text[0] = 'e';
    text[1] = 0;
  // sprintf(text,"hello");

  strcat(text, " world");
  // strcpy(text, "dfssdfsdf");

  // strcpy(text,"world");

  cout << text << endl;
  return 0;
}