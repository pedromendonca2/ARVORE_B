#include "arvoreB.h"

struct arvore{
    int num_chaves; // p/ nós internos e folhas, o min. de chaves é (t/2) − 1 e o max. é ́t − 1, já p/ a raíz, o min. de chaves é 1 e o max. é t − 1
    bool ehFolha;
    int deslocamento; // A posição do nó  no arquivo binário (o deslocamento em bytes para acessar/atualizar este nó dentro do arquivo binario)
    int* chaves; // Este número está no intervalo [⌈t/2⌉−1, t−1] para nós internos e folhas, e entre [1,t−1] para a raiz.
    int* registros; // cada um indexado por uma chave 
    Node** filhos; // (sempre igual ao número de chaves armazenadas + 1)
};

void destroiNode(Node* x){
    if(x == NULL) return;

    for(int i=0; i<x->num_chaves+1; i++){
        destroiNode(x->filhos[i]);
    }

    free(x->chaves);
    free(x->registros);
    free(x->filhos);
    free(x);
}

Node* criaNode(){
    return NULL;
}

Node* criaNode(bool ehFolha, int ordem){
    Node* bt = malloc(sizeof(Node));
    if(bt == NULL){ 
        perror("Memory allocation failed"); 
        exit(1);
    }

    bt->num_chaves = 0;
    bt->ehFolha = ehFolha;

    bt->chaves = malloc((ordem-1) * (sizeof(int)));
    bt->registros = malloc((ordem-1) * (sizeof(int)));
    bt->filhos = malloc(ordem * sizeof(Node*));
    
    for(int i=0; i<ordem; i++){
        bt->filhos[i] = NULL;
    }

    // for (int i = 0; i < ordem - 1; i++) {
    //     bt->chaves[i] = -1;
    //     bt->registros[i] = -1;
    // }

    //bt->deslocamento = 0;

    return bt;
}

void divideFilho(Node* pai, int k, int ordem) {
    Node* filho = pai->filhos[k]; //filho da esquerda

    // printf("Dividindo nó...\n");
    // printf("Antes da divisão, chaves do filho: ");
    // for (int i = 0; i < filho->num_chaves; i++) {
    //     printf("%d ", filho->chaves[i]);
    // }
    // printf("\n");

    Node* novoNode = criaNode(filho->ehFolha, ordem); //cria nó que armazena a metade superior das chaves
    int meio = (ordem - 1) / 2; //(ordem - 1) / 2
    novoNode->num_chaves = filho->num_chaves - meio - 1; //meio - 1
    
    for (int i = 0; i < novoNode->num_chaves; i++) { //copia as chaves e registros da metade superior do nó filho para o novo nó
        novoNode->chaves[i] = filho->chaves[i + meio + 1];
        novoNode->registros[i] = filho->registros[i + meio + 1];
    }
    
    if (!filho->ehFolha) { //se o filho não for folha, os ponteiros para os filhos são copiados para o novo nó
        for (int i = 0; i <= novoNode->num_chaves; i++) {
            novoNode->filhos[i] = filho->filhos[i + meio + 1];
        }
    }

    // printf("Novo nó criado com chaves: ");
    // for (int i = 0; i < novoNode->num_chaves; i++) {
    //     printf("%d ", novoNode->chaves[i]);
    // }
    // printf("\n");
    
    filho->num_chaves = meio;
    
    //printf("Numero de chaves do pai: %d\n", pai->num_chaves);
    for (int i = pai->num_chaves; i > k; i--) { //desloca os ponteiros para os filhos do pai para abrir espaço para o novo nó
        pai->filhos[i + 1] = pai->filhos[i];
    }
    pai->filhos[k + 1] = novoNode;
    
    for (int i = pai->num_chaves - 1; i >= k; i--) { //desloca as chaves do pai para abrir espaço para a chave promovida
        pai->chaves[i + 1] = pai->chaves[i];
        pai->registros[i + 1] = pai->registros[i];
    }
    
    // a chave do meio do nó filho é promovida para o nó pai
    pai->chaves[k] = filho->chaves[meio];
    pai->registros[k] = filho->registros[meio];
    pai->num_chaves++;

    // filho->chaves[meio] = -1;
    // filho->registros[meio] = -1;
}

void insereNonFull(Node* x, int k, int reg, int ordem){
    int i = x->num_chaves - 1;

    if(x->ehFolha){ //se for folha, insere direto
        while(i >= 0 && x->chaves[i] > k){ //desloca as chaves maiores para a direita
            x->chaves[i+1] = x->chaves[i];
            x->registros[i+1] = x->registros[i];
            i--;
        }
        x->chaves[i+1] = k;
        x->registros[i+1] = reg;
        x->num_chaves++;
    } else{ //se não for folha, insere em algum filho
        while(i>=0 && x->chaves[i] > k) i--;
        i++;

        if(x->filhos[i]->num_chaves == ordem - 1){ //se filho estiver cheio, divide e escolhe o filho certo
            divideFilho(x, i, ordem);
            if(x->chaves[i] < k) i++; //decide em qual filho inserir
        }
        insereNonFull(x->filhos[i], k, reg, ordem);
    }
}

Node* insere(Node* x, int k, int reg, int ordem){
    if(x == NULL) { //se a árvore for vazia, cria a sua raíz
        //printf("CRIANDO RAIZ\n");
        Node* raiz = criaNode(true, ordem);
        raiz->registros[0] = reg;
        raiz->chaves[0] = k;
        raiz->num_chaves++;
        //printf("Numero de chaves da raiz: %d\n", raiz->num_chaves);
        //printf("RAIZ CRIADA\n");
        return raiz;
    } else { //insere no mesmo nó se não estiver cheio, e cria outro se estiver
        //printf("INSERINDO NA ARVORE PRONTA\n");
        if(x->num_chaves == ordem-1){ //se a raiz está cheia, cria um novo nó
            Node* novoNode = criaNode(false, ordem);
            novoNode->filhos[0] = x; //o novo nó se torna a raiz e o antigo nó se torna um filho
            divideFilho(novoNode, 0, ordem); //divide o antigo nó
            insereNonFull(novoNode, k, reg, ordem); //insere a chave no novo nó
            return novoNode;
        }
        insereNonFull(x, k, reg, ordem);
        return x;
    }
}

Node* busca(FILE* f, Node* x, int k){
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

void imprime(FILE* f, Node* x){
    fprintf(f, "\n-- ARVORE B\n");

    if(!x) return;

    Queue* q = create_queue();
    enqueue(q, x);
    enqueue(q, NULL);

    while (!is_empty(q)) {
        Node* node = dequeue(q);
        
        if (node == NULL) {
            fprintf(f, "\n");
            if (!is_empty(q)) {
                enqueue(q, NULL); // Adiciona marcador de nível para o próximo nível
            }
        } else {
            fprintf(f, "[");
            for (int i = 0; i < node->num_chaves; i++) {
                fprintf(f, "key: %d, ", node->chaves[i]);
            }
            fprintf(f, "]");
            if (!node->ehFolha) {
                for (int i = 0; i <= node->num_chaves; i++) {
                    if (node->filhos[i]) {
                        enqueue(q, node->filhos[i]);
                    }
                }
            }
        }
    }
    free(q);
}

int retornaNumChaves(Node* x){
    return x->num_chaves;
}