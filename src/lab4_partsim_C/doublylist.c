/* Doubly Linked List implementation */
#include<stdio.h>
#include<stdlib.h>
#include "definitions.h"

struct Node  {
	particle_t particle;
	struct Node* next;
	struct Node* prev;
};
struct List{
  Node *head;
  Node *tail;
  
};

struct Node* head; // global variable - pointer to head node.

//Creates a new Node and returns pointer to it.
 /*
struct Node* GetNewNode(int x) {
	struct Node* newNode
		= (struct Node*)malloc(sizeof(struct Node));
	newNode->data = x;
	newNode->prev = NULL;
	newNode->next = NULL;
	return newNode;
}*/

//Inserts a Node at head of doubly linked list
void InsertAtHead(Node *node, List *list) {
	if(list->head == NULL) {
		list->head = node;
		return;
	}
	list->head->prev = node;
	node->next = list->head; 
	list->head = node;
}

//Inserts a Node at tail of Doubly linked list
void InsertAtTail(Node *node, List *list) {
  
	if(list->head == NULL) {
		list->head = node;
		return;
	}
	list->tail->next = node;
	node->prev = list->tail;
	list->tail = node;
	
}

//Prints all the elements in linked list in forward traversal order
void Print() {
	struct Node* temp = head;
	printf("Forward: ");
	while(temp != NULL) {
		printf("%d ",temp->data);
		temp = temp->next;
	}
	printf("\n");
}

node* remove(node *node, List *list){
  
  if(list->head == node)
    {
      list->head = node->next;      
    }else{
    node->prev->next=node->next;
  }
  if(list->tail == node){
    list->tail = node->prev;
  }else{
    node->next->prev=node->prev;
  }

  return node;
  
}

//Prints all elements in linked list in reverse traversal order. 
void ReversePrint() {
	struct Node* temp = head;
	if(temp == NULL) return; // empty list, exit
	// Going to last Node
	while(temp->next != NULL) {
		temp = temp->next;
	}
	// Traversing backward using prev pointer
	printf("Reverse: ");
	while(temp != NULL) {
		printf("%d ",temp->data);
		temp = temp->prev;
	}
	printf("\n");
}


int main() {


  return 0;	
}
