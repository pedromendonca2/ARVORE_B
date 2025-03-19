#include "fila.h"

struct queueNode {
    BT *node;
    QueueNode *next;
};

struct queue {
    QueueNode *front, *rear;
};

// Funções para manipular a fila
Queue* create_queue() {
    Queue *q = malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    return q;
}

void enqueue(Queue *q, BT *node) {
    QueueNode *newNode = malloc(sizeof(QueueNode));
    newNode->node = node;
    newNode->next = NULL;
    if (q->rear) {
        q->rear->next = newNode;
    } else {
        q->front = newNode;
    }
    q->rear = newNode;
}

BT* dequeue(Queue *q) {
    if (!q->front) return NULL;
    QueueNode *temp = q->front;
    BT *node = temp->node;
    q->front = q->front->next;
    if (!q->front) q->rear = NULL;
    free(temp);
    return node;
}

int is_empty(Queue *q) {
    return q->front == NULL;
}