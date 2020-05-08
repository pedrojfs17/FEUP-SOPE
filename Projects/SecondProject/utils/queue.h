// C program for array implementation of queue based on https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
#include <stdio.h> 
#include <stdlib.h> 
#include <limits.h> 
  
typedef struct queue {
    unsigned front, rear, size, capacity; 
    unsigned* array; 
} queue; 
  
queue createQueue(unsigned capacity) { 
    queue q; 
    q.capacity = capacity; 
    q.front = q.size = 0;  
    q.rear = capacity - 1;
    q.array = (unsigned*) malloc(q.capacity * sizeof(unsigned)); 
    return q; 
} 

int isFull(queue* q) {  return (q->size == q->capacity);  } 

int isEmpty(queue* q) {  return (q->size == 0); } 

void deoccupy(queue* q, unsigned client) { 
    if (isFull(q)) 
        return; 
    q->rear = (q->rear + 1)%q->capacity; 
    q->array[q->rear] = client; 
    q->size++; 
} 

int occupy(queue* q) { 
    if (isEmpty(q)) 
        return 0; 
    unsigned item = q->array[q->front]; 
    q->front = (q->front + 1)%q->capacity; 
    q->size--; 
    return item; 
} 
  
void fillQueue(queue* q){
    for(unsigned i=1;i<=q->capacity;i++){
        deoccupy(q,i);
    }
}