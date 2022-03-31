#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;
void input();

typedef struct listelement {
    char text[1000];

  listelement *next, *prev;

} listelement;

listelement* head = NULL;

void push(){

    cout << "insert text:" << endl;
    char usertext[1000];
    cin >> usertext;
    int data = 5;

	if (head == NULL){
		head = (listelement*)malloc(sizeof(listelement));	
        int i = 0;
        //save the userinput to the listelement
        while(usertext[i] != 0){
            head->text[i] = usertext[i];    //this 'works' for now
            i++;                              //but there must be a better way                     
        }
		head -> next = NULL;
		head -> prev = NULL;
	}
	else{
		listelement *newelem = new listelement;
		listelement *last = NULL;
		for(last = head; last -> next != NULL; last = last->next){
                    cout << last->next << endl;
		}
		last -> next = newelem;
		newelem -> prev = last;
		newelem -> next = NULL;
                //save the userinput to the listelement
        int i = 0;
        while(usertext[i] != 0){
            newelem->text[i] = usertext[i];   //this 'works' for now
            i++;                              //but there must be a better way                     
        }
	}

  cout << "you entered " << usertext << endl;
  cout << "pushed the text" << endl;
  input();


}

void print() {

    for(listelement *p = head; p != NULL; p = p->next ){
        cout << p->text << endl;
    }
  input();
}

//Need to figure out delete and we're good
// Do similar thing to the for loop
// loop until a  certain element is found. 
// Then "remove it" from the chain
// take the previous element's next element and make it 
// the element after's address
// a   b   c     <--- delete b
// a.next = &c
// c.prev = &a

void del() {
  cout << "delete item" << endl;
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

  printf("You entered %i\n", in);

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
    input();
  }
}

int main() {
  input();
  return 0;
}
