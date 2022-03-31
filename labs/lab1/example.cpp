#include <stdio.h>
#include <iostream>
#include <string>

// void fct(int m){ //m == a; 
// 	m = 99;
// }

using namespace std;

typedef struct listelem{
	//data always in order
	int i; //ex address 16
	int y; //ex address 20
	//linking
	listelem *n, *p;
	
} listelem;


listelem *head = NULL;

void new_elem(){
	if (head == NULL){
		head = malloc(sizeof(listelem));	
		head -> i = data;
		head -> n = NULL;
		head -> p = NULL;
	}
	else{
		listelem *newelem = (listelem *)malloc (sizeof(listelem));
		listelem *last = NULL;
		for(last = head; last -> n != NULL; last -> n){

		}
		last -> n = newelem;
		newelem -> p = last;
		newelem -> n = NULL;
		newelem -> i = data;
	}
}



int main(){
	

	int a;
	int b;
	float g;

	a = 7;
	b = 55;

	int *p;
	int* k, a; //k is a pointer, a is an integer
	p = &a; //address of a 8 in ex
	p = &b; //address of a 16 in ex
	*p = 99; //dereference op (access the content of the pointer p and change the content of the pointer to 99)


	fct(&a);
	cout << a << endl; //cpp  prints 7 if don't use pointers




	listelem a, b, *o;
	a.i = 9;
	a.y = 7;
	int *p, *k;
	p = &a.i;
	k = &a.y;
	o = &a;  //entire structure starts at a.i (first space in struct)


	


	int arr[10];
	int *u, *e;
	u = &arr[10];
	e = arr; // arr = &arr[0]



	listelem a, b, c;


	a.i = 9;
	b.i = 10;
	c.i = 11;

	a.n = &b;
	b.p= &a;
	c.p = &b;
	c.n = NULL;
	a.p = NULL;
	b.n = NULL;
	c.n = NULL;


	for(listelem *p = &a; p != NULL; p = (*p).n){ //(*p).n) == p -> n
		cout << (*p).i <<endl;
	}



	int *ar = (int*) malloc(100000 * 4); //for dynamically allocating space in memory
	ar[70] = 88; // == *(ar + 70*4)




	newelem(8);
	newelem(7);
	newelem(6);
	newelem(5);



	int *p = (int *) maclloc (sizeof(int));
	free(p);

	//cpp version below
	int *p = new int;
	delete p;
	// 



	int h;
	h = 0;


	return 0;
	}
