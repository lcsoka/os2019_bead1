#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "passenger.h"
#include <string.h>

Node * createnode(PASSENGER* data);

Node * createnode(PASSENGER* data){
  Node * newNode = malloc(sizeof(Node));
  if (!newNode) {
    return NULL;
  }
  newNode->data = data;
  newNode->next = NULL;
  return newNode;
}

List * initList(){
  List * list = malloc(sizeof(List));
  if (!list) {
    return NULL;
  }
  list->head = NULL;
  return list;
}

PASSENGER * getNthPassenger(int index, List * list) {
  Node * current = list->head;
  // List is empty
  if(list->head == NULL){
    return NULL;
  }

  // Index bigger than actual list
  if (count(list) < index){
    return NULL;
  } 
  int i = 0;
  PASSENGER * passenger;
  for(; current != NULL; current = current->next) {
    i++;
    if(i == index){
      passenger = current->data;
    }
  }
  return passenger;
}

void add(PASSENGER * data, List * list){
  Node * current = NULL;
  if(list->head == NULL)
  {
    list->head = createnode(data);
  } else {
    current = list->head;
    while (current->next!=NULL){
      current = current->next;
    }
    current->next = createnode(data);
  }
}

void deleteFromList(PASSENGER * data, List * list){
  Node * current = list->head;            
  Node * previous = current;           
  while(current != NULL){           
    if(current->data == data){      
      previous->next = current->next;
      if(current == list->head)
        list->head = current->next;
      free(current);
      return;
    }                               
    previous = current;             
    current = current->next;        
  }                                 
}                                   

void reverse(List * list){
  Node * reversed = NULL;
  Node * current = list->head;
  Node * temp = NULL;
  while(current != NULL){
    temp = current;
    current = current->next;
    temp->next = reversed;
    reversed = temp;
  }
  list->head = reversed;
}

void destroy(List * list){
  Node * current = list->head;
  Node * next = current;
  while(current != NULL){
    next = current->next;
    free(current);
    current = next;
  }
  free(list);
}

int count(List * list) 
{
  Node * current = list->head;
  if(list->head == NULL) 
    return 0;
  int c = 0;
  for(; current != NULL; current = current->next) {
    c++;
  }
  return c;
}