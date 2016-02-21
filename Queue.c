#include <stdlib.h>
#include "Queue.h"

Node NULL_NODE = {-1, -1, -1, -1};

Queue Q_init()
{
    Queue q;
    q.head = NULL;
    q.tail = NULL;
    return q;
}

void Q_enqueue(Queue* q, Node n)
{
    if (q->head == NULL)
    {
        q->nodes[0] = n;
        q->head = &q->nodes[0];
        q->tail = &q->nodes[0];
    }
    else
    {
        q->tail++;
        if(q->tail > &q->nodes[MAX_QUEUE_LENGTH - 1]) q->tail = &q->nodes[0];
        if(q->tail != q->head)
        {
            *(q->tail) = n;
        }
        else
        {
            printf("Queue is full\n");
            q->tail--;
            if(q->tail < &q->nodes[0]) q->tail = &q->nodes[MAX_QUEUE_LENGTH - 1];
        }
    }
    //printf("head points to %#0x8, tail points to %#0x8\n", q->head, q->tail);
}

Node Q_dequeue(Queue* q)
{
    Node res = NULL_NODE;
    if(q->head != NULL)
    {
        res = *(q->head);
        q->head++;
        if(q->head > &q->nodes[MAX_QUEUE_LENGTH - 1]) q->head = &q->nodes[0];
        if(q->head == q->tail+1 || (q->tail == &q->nodes[MAX_QUEUE_LENGTH - 1] && q->head == &q->nodes[0]))
        {
            q->head = NULL;
            q->tail = NULL;
        }
    }else
    {
        printf("Empty queue!\n");
    }
    return res;
}

bool Q_notEmpty(Queue* q)
{
    bool res = q->head == NULL ? false : true;
    return res;
}
