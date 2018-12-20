#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//结点结构
typedef struct node{
  int data;	//储存数据
  struct node *next;	//下一个结点
} ListNode, *ListPtr;
//		创建链表
ListPtr CreateList(ListPtr P)
{
    int stu;
    char choose;
    do{
    printf("Do you want to create a list?(Yes or No).\n");
    scanf("%c",&choose);
    printf("Input data\n");
    scanf("%d",&stu);
    P = (ListPtr)malloc(sizeof(ListNode));
    P->data = stu;
    P = P->next;
    }while(choose == 'Y');
    return P;
}
//		插入链表
void InsertList (ListPtr P,ListPtr L)
{
    int e;
    L = (ListPtr)malloc(sizeof(ListNode));
    printf("Input the data that you want to insert.\n");
    scanf("%d",&e);
    L->data = e;
    P->next = L;
    L->next = NULL;
}
//		打印链表
void PrintList(ListPtr P)
{
    while(P){
    printf("The data is %d",P->data);
    P = P->next;
}
}
int main(void)
{
   ListPtr P,L;
   P = (ListPtr)malloc(sizeof(ListNode));
   L = (ListPtr)malloc(sizeof(ListNode));
   while(1)
   {
		printf("1 create list\n");
		printf("2 printf list\n");
		printf("3 insert list\n");
		printf("4 quit\n");
		char command =getchar();
		switch(command)
		{
  		case '1' : P = CreateList(P);
  			break;
 			case '2' : PrintList(P);
  			break;
	 	  case '3' : InsertList(P,L);
				break;
 			default: 
				return 0;
		}
	}
}
