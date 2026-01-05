#include "queue.h"

void initializeQueue(Queue* q) {
    q->first = 0;
    q->last = 0;
    q->size = 0;
}

bool isFull(const Queue* q) {
    return q->size == MAX_SIZE;
}

bool isEmpty(const Queue* q) {
    return q->size == 0;
}

void enqueue(Queue* q, int value) {
    if (isFull(q)) {
        printf("Queue is full. Cannot enqueue %d\n", value);
        return;
    }
    q->items[q->last] = value;
    q->last = (q->last + 1) % MAX_SIZE;
    q->size++;
}

int dequeue(Queue* q) {
    if (isEmpty(q)) {
        printf("Queue is empty. Cannot dequeue\n");
        return -1; // Indicating that the queue is empty
    }
    int value = q->items[q->first];
    q->first = (q->first + 1) % MAX_SIZE;
    q->size--;
    return value;
}