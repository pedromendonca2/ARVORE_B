#ifndef FILA_H
#define FILA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct arvore BT;
typedef struct queue Queue;
typedef struct queueNode QueueNode;

Queue* create_queue();

void enqueue(Queue *q, BT *node);

BT* dequeue(Queue *q);

int is_empty(Queue *q);

#endif