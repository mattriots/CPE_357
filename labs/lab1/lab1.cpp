#include <stdio.h>

#include <iostream>
#include <string>

using namespace std;
void input();

typedef struct listelement {
  char text[1000];

  listelement *prev, *next;

} listelement;

listelement* head = NULL;

void push() {
  cout << "insert text:" << endl;
  char usertext[1000];
  cin >> usertext;

  if (head == NULL) {
    head = (listelement*)malloc(sizeof(listelement));
    head->next = NULL;
    head->prev = NULL;
    int i = 0;

    // save the userinput to the listelement TEXT
    while (usertext[i] != 0) {
      head->text[i] = usertext[i];  // this 'works' for now
      i++;                          // but there must be a better way
    }

  } else {
    listelement* newelem = new listelement;
    listelement* last = NULL;
    for (last = head; last->next != NULL; last = last->next) {
    }

    last->next = newelem;
    newelem->prev = last;
    newelem->next = NULL;

    // save the userinput to the listelement TEXT
    int i = 0;
    while (usertext[i] != 0) {
      newelem->text[i] = usertext[i];  // this 'works' for now
      i++;                             // but there must be a better way
    }
  }

  cout << "you entered " << usertext << endl;
  cout << "pushed the text" << endl;
  input();
}

void print() {
  int i = 1;
  cout << "------------------" << endl;
  for (listelement* p = head; p != NULL; p = p->next) {
    cout << i << " : " << p->text << endl;
    i++;
  }
  cout << "------------------" << endl;
  input();
}

void del() {
  int i = 1;
  cout << "------------------" << endl;
  for (listelement* p = head; p != NULL; p = p->next) {
    cout << i << " : " << p->text << endl;
    i++;
  }
  cout << "------------------" << endl;
  cout << "enter an item to delete:" << endl;

  int usernum;
  cin >> usernum;

  int j = 1;
  for (listelement* p = head; p != NULL; p = p->next) {
    string s = p->text;
    if (j == usernum) {
      //
      if (p == head) {
        head = p->next;
        input();
      } else if (p->next == NULL) {
        listelement* previous = p->prev;

        previous->next = NULL;
        int* t = new int;
        delete t;
        input();
      } else {
        listelement* previous = p->prev;
        listelement* nextel = p->next;
        previous->next = nextel;
        nextel->prev = previous;
        input();
      }
    }
    
    j++;
  }

  cout << "item does not exist" << endl;
  input();
}
int end() {
  cout << "ending program" << endl;
  return 0;
}

void input() {
  cout << "Select" << endl;
  cout << "1 push string" << endl;
  cout << "2 print list" << endl;
  cout << "3 delete item" << endl;
  cout << "4 end program" << endl;
  int in;
  scanf("%i", &in);

  if (in == 1) {
    push();

  } else if (in == 2) {
    print();

  } else if (in == 3) {
    del();

  } else if (in == 4) {
    end();
  } else {
    cout << "not a valid input" << endl;
    return;
   
  }
}

int main() {
  input();
  return 0;
}
