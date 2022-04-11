#include <iostream>
#include <string.h>

using namespace std;


class lle{
    public:
    int i;
    lle *next;
    lle *prev;
    lle(){
        next = NULL;
        prev = NULL;
    }
};

typedef unsigned char BYTE;

int main()
{
    lle  *head = NULL;
    for(;;){
        int input;
        cin >> input;
        if(input == 0) {
            //remove (free or delete) every list element before ending program 
            return 0;
        }
        if (input == 1){
            for(lle *p = head; p != NULL; p=p->next){
                cout << p->i << endl;
            }
        }//print list
        if (input == 2){
            lle *n = new lle;
            head = n; // if head == NULL
            //else
            //find the last elem
            lle *p =NULL;
            for(p = head; p->next != NULL; p=p->next); //find the last element
            p->next = n; 
            n->prev = p;  //link the prev and next
        }
    }

    return 0;
}