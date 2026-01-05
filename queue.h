#include <stdbool.h>
#include <stdio.h>
#define MAX_SIZE 64

typedef struct {
    int items[MAX_SIZE];
    int first;
    int last;
    int size;
} Queue;

void initializeQueue(Queue* q);
bool isFull(const Queue* q);
bool isEmpty(const Queue* q);
void enqueue(Queue* q, int value);
int dequeue(Queue* q);