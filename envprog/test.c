# include <stdio.h>
void main()
{
    int a;
    a = 9;
    int arr[10];
    int i;
    for(i=0;i<10;i++)
        arr[i] = i*2;

    printf("%d\n",arr[1]);
    printf("I am alive");
}