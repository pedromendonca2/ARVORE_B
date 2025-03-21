#ifndef ARVORE_B
#define ARVORE_B

#include "fila.h"

typedef struct arvore Node;

void destroiNode(Node* x);

Node* criaNode();

Node* criaNode(bool ehFolha, int ordem);

void divideFilho(Node* pai, int k, int ordem);

void insereNonFull(Node* x, int k, int reg, int ordem);

Node* insere(Node* x, int k, int reg, int ordem);

Node* busca(FILE* f, Node* x, int k);

void imprime(FILE* f, Node* x);

int retornaNumChaves(Node* x);

#endif