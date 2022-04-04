#include <stdio.h>

#include <iostream>
#include <string>

using namespace std;
void input();

typedef struct listelement
{
  char text[1000];

  listelement *prev, *next;

} listelement;

listelement *head = NULL;

void push()
{
  cout << "insert text:" << endl;
  char usertext[1000];
  cin >> usertext;

  if (head == NULL)
  {
    head = (listelement *)malloc(sizeof(listelement));
    head->next = NULL;
    head->prev = NULL;
    int i = 0;

    // save the userinput to the listelement TEXT
    while (usertext[i] != 0)
    {
      head->text[i] = usertext[i]; // this 'works' for now
      i++;                         // but there must be a better way
    }
  }
  else
  {
    listelement *newelem = new listelement;
    listelement *last = NULL;
    for (last = head; last->next != NULL; last = last->next);
   
    last->next = newelem;
    newelem->prev = last;
    newelem->next = NULL;

    // save the userinput to the listelement TEXT
    int i = 0;
    while (usertext[i] != 0)
    {
      newelem->text[i] = usertext[i]; // this 'works' for now
      i++;                            // but there must be a better way
    }
  }

  cout << "pushed the text" << endl;
  input();
}

void print()
{
  int i = 1;
  cout << "------------------" << endl;
  for (listelement *p = head; p != NULL; p = p->next)
  {
    cout << i << " : " << p->text << endl;
    i++;
  }
  cout << "------------------" << endl;
  input();
}

void del()
{
  int i = 1;
  cout << "------------------" << endl;
  for (listelement *p = head; p != NULL; p = p->next)
  {
    cout << i << " : " << p->text << endl;
    i++;
  }
  cout << "------------------" << endl;
  cout << "enter an item to delete:" << endl;

  int usernum;
  cin >> usernum;

  int j = 1;
  for (listelement *p = head; p != NULL; p = p->next)
  {
    if (j == usernum)
    {
      //removing first element of list
      if (p == head)
      {
        head = p->next;
        delete (p);
        input();
      }
      //removing last element of list
      else if (p->next == NULL)
      {

        p->prev->next = NULL;
        delete (p);
        input();
      }
      //removing elements from the middle
      else
      {
        listelement *prev = p->prev;
        listelement *next = p->next;
        listelement *current = p;

        prev->next = p->next;
        next->prev = p->prev;
        delete (current);
        input();
      }
    }

    j++;
  }

  cout << "item does not exist" << endl;
  input();
}
void end()
{
  cout << "ending program" << endl;
  return;
}

void input()
{
  cout << "Select" << endl;
  cout << "1 push string" << endl;
  cout << "2 print list" << endl;
  cout << "3 delete item" << endl;
  cout << "4 end program" << endl;
  int in;
  scanf("%i", &in);

  if (in == 1)
  {
    push();
  }
  else if (in == 2)
  {
    print();
  }
  else if (in == 3)
  {
    del();
  }
  else if (in == 4)
  {
    end();
  }
  else
  {
    cout << "not a valid input" << endl;
    return;
  }

  return;
}

int main()
{
  input();
  return 0;
}
