/* Doubly Linked List implementation */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "definitions.h"

typedef struct Node_t  {
  particle_t particle;
  struct Node_t* next;
  struct Node_t* prev;
} Node;
typedef struct List_t{
  Node *head;
  Node *tail;
  
}List;



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
  Node * newNode = (Node*)malloc(sizeof(Node));
  memcpy(newNode,node,sizeof(Node));
  if(list->head == NULL) {
    list->head = newNode;
    list->tail = newNode;
    return;
  }
  list->tail->next = newNode;
  newNode->prev = list->tail;
  list->tail = newNode;
	
}

//Prints all the elements in linked list in forward traversal order
void Print(List *list) {
  Node* temp = list->head;
  printf("Forward: ");
  while(temp != NULL) {
    printf("%f ",temp->particle.pcord.vx);
    temp = temp->next;
  }
  printf("\n");
}

Node* RemoveNode(Node *node, List *list){
  
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
/*
void ReversePrint() {
  Node* temp = list->head;
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
*/

