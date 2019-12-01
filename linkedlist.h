#include "passenger.h"

typedef struct node {
  PASSENGER* data;
  struct node * next;
} Node;

typedef struct list {
  Node * head; 
} List;

List * initList();
void add(PASSENGER* data, List * list);
PASSENGER * getNthPassenger(int index, List * list);
PASSENGER * getPassengerWithId(int id, List * list);
void deleteFromList(PASSENGER* data, List * list);
void reverse(List * list);
void destroy(List * list);
int count(List * list);
