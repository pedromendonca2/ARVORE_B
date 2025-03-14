#include "arvoreB.h"

struct arvore{
    int num_chaves; // p/ nós internos e folhas, o min. de chaves é (t/2) − 1 e o max. é ́t − 1, já p/ a raíz, o min. de chaves é 1 e o max. é t − 1
    bool ehFolha;
    int deslocamento; // A posição do nó  no arquivo binário (o deslocamento em bytes para acessar/atualizar este nó dentro do arquivo binario)
    int* chaves; // Este número está no intervalo [⌈t/2⌉−1, t−1] para nós internos e folhas, e entre [1,t−1] para a raiz.
    int* registros; // cada um indexado por uma chave 
    BT** filhos; // (sempre igual ao número de chaves armazenadas + 1)
};

void destroiBT(BT* x){
    if(x == NULL) return;

    for(int i=0; i<x->num_chaves+1; i++){
        destroiBT(x->filhos[i]);
    }

    free(x->chaves);
    free(x->registros);
    free(x->filhos);
    free(x);
}

BT* criaBT(){
    return NULL;
}

BT* criaNode(bool ehFolha, int ordem){
    BT* bt = malloc(sizeof(BT));
    if(bt == NULL){ 
        perror("Memory allocation failed"); 
        exit(1);
    }

    bt->num_chaves = 0;
    bt->ehFolha = ehFolha;

    bt->chaves = malloc((ordem-1) * (sizeof(int)));
    bt->registros = malloc((ordem-1) * (sizeof(int)));
    bt->filhos = malloc(ordem * sizeof(BT*));
    for(int i=0; i<ordem; i++){
        bt->filhos[i] = NULL;
    }

    //bt->deslocamento = 0;

    return bt;
}

void divideFilho(BT* pai, int k, int ordem) {
    BT* filho = pai->filhos[k];
    BT* novoNode = criaNode(filho->ehFolha, ordem);
    
    novoNode->num_chaves = ordem/2 - 1;
    
    for (int i = 0; i < ordem/2 - 1; i++) {
        novoNode->chaves[i] = filho->chaves[i + ordem/2];
        novoNode->registros[i] = filho->registros[i + ordem/2];

    }
    
    if (!filho->ehFolha) {
        for (int i = 0; i < ordem/2; i++) {
            novoNode->filhos[i] = filho->filhos[i + ordem/2];
        }
    }
    
    filho->num_chaves = ordem/2 - 1;
    
    for (int i = pai->num_chaves; i > k; i--) {
        pai->filhos[i + 1] = pai->filhos[i];
    }
    
    pai->filhos[k + 1] = novoNode;
    
    for (int i = pai->num_chaves - 1; i >= k; i--) {
        pai->chaves[i + 1] = pai->chaves[i];
        pai->registros[i + 1] = pai->registros[i];
    }
    
    pai->chaves[k] = filho->chaves[ordem/2 - 1];
    pai->registros[k] = filho->registros[ordem/2 - 1];
    pai->num_chaves++;
}

void insereNonFull(BT* x, int k, int reg, int ordem){
    int i = x->num_chaves - 1;

    if(x->ehFolha){
        while(i >= 0 && x->chaves[i] > k){
            x->chaves[i+1] = x->chaves[i];
            x->registros[i+1] = x->registros[i];
            i--;
        }
        x->chaves[i+1] = k;
        x->registros[i+1] = reg;
        x->num_chaves++;
    } else{
        while(i>=0 && x->chaves[i] > k) i--;
        i++;

        if(x->filhos[i]->num_chaves == ordem - 1){
            divideFilho(x, i, ordem);

            if(x->chaves[i] < k) i++;
        }
        insereNonFull(x->filhos[i], k, reg, ordem);
    }
}

BT* insere(BT* x, int k, int reg, int ordem){
    if(x == NULL) {
        printf("CRIANDO RAIZ\n");
        BT* raiz = criaNode(true, ordem);
        raiz->registros[0] = reg;
        raiz->chaves[0] = k;
        raiz->num_chaves++;
        printf("Numero de chaves da raiz: %d\n", raiz->num_chaves);
        printf("RAIZ CRIADA\n");
        return raiz;
    } else {
        printf("INSERINDO NA ARVORE PRONTA\n");
        if(x->num_chaves == ordem-1){
            BT* novoNode = criaNode(false, ordem);
            novoNode->filhos[0] = x;
            divideFilho(novoNode, 0, ordem);
        }
        insereNonFull(x, k, reg, ordem);
        return x;
    }
}

BT* busca(FILE* f, BT* x, int k){
    int i = 0; //printf("CHAVE: %d\n", k);

    while(i < x->num_chaves && k > x->chaves[i]) i++;

    if(i < x->num_chaves && k == x->chaves[i]){
        fprintf(f, "O REGISTRO ESTA NA ARVORE!\n");
        return x;
    }

    if(x->ehFolha){
        fprintf(f, "O REGISTRO NAO ESTA NA ARVORE!\n");
        return NULL;
    }
    
    return busca(f, x->filhos[i], k);
}

void imprime(FILE* f, BT* x){
    fprintf(f, "\n-- ARVORE B\n");

    if(!x) return;

    Queue* q = create_queue();
    enqueue(q, x);
    while (!is_empty(q)) {
        BT* node = dequeue(q);
        fprintf(f, "[");
        //printf("NUM CHAVES: %d\n", node->num_chaves);
        for (int i = 0; i < node->num_chaves; i++) {
            fprintf(f, "key: %d, ", node->chaves[i]);
        }
        fprintf(f, "]");
        if (!node->ehFolha) {
            fprintf(f, "\n");
            for (int i = 0; i <= node->num_chaves; i++) {
                if (node->filhos[i]) {
                    enqueue(q, node->filhos[i]);
                }
            }
        }
    }
    free(q);
}

int retornaNumChaves(BT* x){
    return x->num_chaves;
}