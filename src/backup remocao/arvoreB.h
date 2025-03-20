#ifndef ARVORE_B
#define ARVORE_B

#include "fila.h"

typedef struct arvore BT;

void destroiBT(BT* x);

BT* criaBT();

BT* criaNode(bool ehFolha, int ordem);

void divideFilho(BT* pai, int k, int ordem);

void insereNonFull(BT* x, int k, int reg, int ordem);

BT* insere(BT* x, int k, int reg, int ordem);

BT* busca(FILE* f, BT* x, int k);

void imprime(FILE* f, BT* x);

int retornaNumChaves(BT* x);


BT* removeKey(BT* root, int k, int ordem);
#endif