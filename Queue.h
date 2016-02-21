#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>

//Queue to use for BFS search
#define ROOT_NODE_PARENT_DIRECTION -3
#define MAX_QUEUE_LENGTH 10

typedef struct Node
{
    int level;
    int parentDirection;
    int x, y;
} Node;

extern Node NULL_NODE;

typedef struct Queue
{
    Node nodes[MAX_QUEUE_LENGTH];
    Node* head;
    Node* tail;
} Queue;

Queue Q_init();

void Q_enqueue(Queue* q, Node n);

Node Q_dequeue(Queue* q);

bool Q_notEmpty(Queue* q);

#endif // QUEUE_H
