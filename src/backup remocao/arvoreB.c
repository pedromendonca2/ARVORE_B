#include "arvoreB.h"

struct arvore{
    int num_chaves; // p/ nós internos e folhas, o min. de chaves é (t/2) − 1 e o max. é ́t − 1, já p/ a raíz, o min. de chaves é 1 e o max. é t − 1
    bool ehFolha;
    int deslocamento; // A posição do nó  no arquivo binário (o deslocamento em bytes para acessar/atualizar este nó dentro do arquivo binario)
    int* chaves; // Este número está no intervalo [⌈t/2⌉−1, t−1] para nós internos e folhas, e entre [1,t−1] para a raiz.
    int* registros; // cada um indexado por uma chave 
    BT** filhos; // (sempre igual ao número de chaves armazenadas + 1)
};

void removeRec(BT* node, int k, int ordem);

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

    // for (int i = 0; i < ordem - 1; i++) {
    //     bt->chaves[i] = -1;
    //     bt->registros[i] = -1;
    // }

    //bt->deslocamento = 0;

    return bt;
}

void divideFilho(BT* pai, int k, int ordem) {
    BT* filho = pai->filhos[k]; //filho da esquerda

    // printf("Dividindo nó...\n");
    // printf("Antes da divisão, chaves do filho: ");
    // for (int i = 0; i < filho->num_chaves; i++) {
    //     printf("%d ", filho->chaves[i]);
    // }
    // printf("\n");

    BT* novoNode = criaNode(filho->ehFolha, ordem); //cria nó que armazena a metade superior das chaves
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

void insereNonFull(BT* x, int k, int reg, int ordem){
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

BT* insere(BT* x, int k, int reg, int ordem){
    if(x == NULL) { //se a árvore for vazia, cria a sua raíz
        //printf("CRIANDO RAIZ\n");
        BT* raiz = criaNode(true, ordem);
        raiz->registros[0] = reg;
        raiz->chaves[0] = k;
        raiz->num_chaves++;
        //printf("Numero de chaves da raiz: %d\n", raiz->num_chaves);
        //printf("RAIZ CRIADA\n");
        return raiz;
    } else { //insere no mesmo nó se não estiver cheio, e cria outro se estiver
        //printf("INSERINDO NA ARVORE PRONTA\n");
        if(x->num_chaves == ordem-1){ //se a raiz está cheia, cria um novo nó
            BT* novoNode = criaNode(false, ordem);
            novoNode->filhos[0] = x; //o novo nó se torna a raiz e o antigo nó se torna um filho
            divideFilho(novoNode, 0, ordem); //divide o antigo nó
            insereNonFull(novoNode, k, reg, ordem); //insere a chave no novo nó
            return novoNode;
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
    enqueue(q, NULL);

    while (!is_empty(q)) {
        BT* node = dequeue(q);
        
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

int retornaNumChaves(BT* x){
    return x->num_chaves;
}

// Funções de Remoção (Implementação Completa)

// Função auxiliar: Encontra o predecessor de uma chave em um nó não folha
int encontraPredecessor(BT* node, int index) {
    BT* current = node->filhos[index];
    while (!current->ehFolha) {
        current = current->filhos[current->num_chaves]; // Vai para o filho mais à direita
    }
    return current->chaves[current->num_chaves - 1]; // Retorna a chave mais à direita (maior)
}

// Função auxiliar: Encontra o sucessor de uma chave em um nó não folha
int encontraSucessor(BT* node, int index) {
    BT* current = node->filhos[index + 1];
    while (!current->ehFolha) {
        current = current->filhos[0]; // Vai para o filho mais à esquerda
    }
    return current->chaves[0]; // Retorna a chave mais à esquerda (menor)
}

// Função auxiliar:  Mescla dois filhos de um nó
void mergeFilhos(BT* node, int index, int ordem) {
    BT* filhoEsquerda = node->filhos[index];
    BT* filhoDireita = node->filhos[index + 1];

    // Move a chave do pai para o final do filho da esquerda
    filhoEsquerda->chaves[filhoEsquerda->num_chaves] = node->chaves[index];
    filhoEsquerda->registros[filhoEsquerda->num_chaves] = node->registros[index];
    filhoEsquerda->num_chaves++;


    // Copia as chaves e registros do filho da direita para o filho da esquerda
    for (int i = 0; i < filhoDireita->num_chaves; i++) {
        filhoEsquerda->chaves[filhoEsquerda->num_chaves] = filhoDireita->chaves[i];
        filhoEsquerda->registros[filhoEsquerda->num_chaves] = filhoDireita->registros[i];
        filhoEsquerda->num_chaves++;
    }

    // Copia os ponteiros dos filhos (se não for folha)
    if (!filhoEsquerda->ehFolha) {
        for (int i = 0; i <= filhoDireita->num_chaves; i++) {
            filhoEsquerda->filhos[filhoEsquerda->num_chaves] = filhoDireita->filhos[i];
        }
    }

    // Desloca as chaves e filhos do nó pai para a esquerda, para "remover" a chave e o ponteiro do filho da direita
    for (int i = index + 1; i < node->num_chaves; i++) {
        node->chaves[i - 1] = node->chaves[i];
        node->registros[i - 1] = node->registros[i];
        node->filhos[i] = node->filhos[i + 1];
    }
    node->num_chaves--;
    
    // Libera a memória do filho da direita (já que agora ele é parte do da esquerda)
    free(filhoDireita->chaves);
    free(filhoDireita->registros);
    free(filhoDireita->filhos);
    free(filhoDireita);
}



// Função auxiliar: Remove uma chave de um nó folha
void removeDeFolha(BT* node, int index) {
    // Desloca todas as chaves e registros após o índice para a esquerda
    for (int i = index + 1; i < node->num_chaves; i++) {
        node->chaves[i - 1] = node->chaves[i];
        node->registros[i - 1] = node->registros[i];
    }
    node->num_chaves--; // Decrementa o número de chaves
}

// Função auxiliar: Remove uma chave de um nó não folha
void removeDeNaoFolha(BT* node, int index, int ordem) {
    int k = node->chaves[index];

    // Caso 1: Se o filho que precede k (filhos[index]) tem pelo menos t chaves
    if (node->filhos[index]->num_chaves >= ordem/2) {
        int pred = encontraPredecessor(node, index);
        node->chaves[index] = pred; // Substitui k pelo predecessor
        node->registros[index] = pred;
        removeRec(node->filhos[index], pred, ordem);  // Remove recursivamente o predecessor
    }
    // Caso 2: Se o filho que sucede k (filhos[index+1]) tem pelo menos t chaves
    else if (node->filhos[index + 1]->num_chaves >= ordem/2) {
        int succ = encontraSucessor(node, index);
        node->chaves[index] = succ; // Substitui k pelo sucessor
        node->registros[index] = succ;
        removeRec(node->filhos[index + 1], succ, ordem); // Remove recursivamente o sucessor
    }
    // Caso 3: Se ambos os filhos têm t-1 chaves, mescla-os e remove k do nó mesclado
     else {
        mergeFilhos(node, index, ordem);
        removeRec(node->filhos[index], k, ordem);
    }
}

// Função auxiliar: Empresta uma chave do irmão da esquerda
void emprestaDoAnterior(BT* node, int index) {

    BT* filho = node->filhos[index];
    BT* irmao = node->filhos[index - 1];

    // Desloca todas as chaves em filho uma posição para a direita
    for (int i = filho->num_chaves - 1; i >= 0; i--) {
        filho->chaves[i + 1] = filho->chaves[i];
        filho->registros[i + 1] = filho->registros[i];
    }

    // Se filho não for folha, move os ponteiros dos filhos também
    if (!filho->ehFolha) {
        for (int i = filho->num_chaves; i >= 0; i--) {
            filho->filhos[i + 1] = filho->filhos[i];
        }
    }

    // Move a chave do pai para a primeira posição do filho
    filho->chaves[0] = node->chaves[index - 1];
    filho->registros[0] = node->registros[index -1];

    // Move a última chave do irmão para o pai
    node->chaves[index - 1] = irmao->chaves[irmao->num_chaves - 1];
    node->registros[index - 1] = irmao->registros[irmao->num_chaves -1];


    // Move o último filho do irmão para ser o primeiro filho de 'filho'
    if (!filho->ehFolha) {
        filho->filhos[0] = irmao->filhos[irmao->num_chaves];
    }

    filho->num_chaves++;
    irmao->num_chaves--;
}

// Função auxiliar: Empresta uma chave do irmão da direita
void emprestaDoProximo(BT* node, int index) {

    BT* filho = node->filhos[index];
    BT* irmao = node->filhos[index + 1];

    // Move a chave do pai para a última posição do filho
    filho->chaves[filho->num_chaves] = node->chaves[index];
    filho->registros[filho->num_chaves] = node->registros[index];


    // Move a primeira chave do irmão para o pai
    node->chaves[index] = irmao->chaves[0];
    node->registros[index] = irmao->registros[0];

    // Move o primeiro filho do irmão para ser o último filho de 'filho'
    if (!filho->ehFolha) {
        filho->filhos[filho->num_chaves + 1] = irmao->filhos[0];
    }

    // Desloca todas as chaves do irmão uma posição para a esquerda
    for (int i = 1; i < irmao->num_chaves; i++) {
        irmao->chaves[i - 1] = irmao->chaves[i];
        irmao->registros[i - 1] = irmao->registros[i];
    }

    // Se irmão não for folha, move os ponteiros dos filhos também
    if (!irmao->ehFolha) {
        for (int i = 1; i <= irmao->num_chaves; i++) {
            irmao->filhos[i - 1] = irmao->filhos[i];
        }
    }

    filho->num_chaves++;
    irmao->num_chaves--;
}



// Função auxiliar: Garante que um nó tenha pelo menos t chaves após uma remoção
void preenche(BT* node, int index, int ordem) {
    //Se o filho anterior (index-1) tiver mais de t-1 chaves, empresta dele
    if (index != 0 && node->filhos[index - 1]->num_chaves >= ordem/2 ) {
        emprestaDoAnterior(node, index);
    }
    //Se o próximo filho (index+1) tiver mais de t-1 chaves, empresta dele
    else if (index != node->num_chaves && node->filhos[index + 1]->num_chaves >= ordem/2) {
        emprestaDoProximo(node, index);
    }
    //Do contrário, mescla o filho com seu irmão
    else {
        if (index != node->num_chaves) {
            mergeFilhos(node, index, ordem);
        } else {
            mergeFilhos(node, index - 1, ordem);
        }
    }
}



// Função principal de remoção (recursiva)
void removeRec(BT* node, int k, int ordem) {
    int index = 0;
    while (index < node->num_chaves && k > node->chaves[index]) {
        index++;
    }

    // A chave a ser removida está presente neste nó
    if (index < node->num_chaves && k == node->chaves[index]) {
        if (node->ehFolha) {
            removeDeFolha(node, index);
        } else {
            removeDeNaoFolha(node, index, ordem);
        }
    } else {
        // Se este nó é uma folha, então a chave não está na árvore
        if (node->ehFolha) {
            printf("A chave %d não existe na árvore.\n", k);
            return;
        }

        // A chave pode estar presente no filho[index]
        bool flag = (index == node->num_chaves); //flag indica se o filho onde a chave pode existir é o último filho do nó

        // Se o filho[index] tem menos de t chaves, preenche-o
        if (node->filhos[index]->num_chaves < ordem/2) {
            preenche(node, index, ordem);
        }

        
        if (flag && index > node->num_chaves) { //se o nó foi mesclado, o filho[index] não existe mais
            removeRec(node->filhos[index - 1], k, ordem);
        }else{
             removeRec(node->filhos[index], k, ordem);
        }
    }
}

// Função pública para remover uma chave da Árvore B
BT* removeKey(BT* root, int k, int ordem) {
    printf("Começando a remoção\n");
    if (!root) {
        printf("A árvore está vazia.\n");
        return root;
    }

    removeRec(root, k, ordem);

    // Se a raiz tiver 0 chaves após a remoção, torna o primeiro filho a nova raiz (se existir)
    if (root->num_chaves == 0) {
        BT* tmp = root;
        if (root->ehFolha) {
            root = NULL;
        } else {
            root = root->filhos[0];
        }
        free(tmp->chaves);
        free(tmp->registros);
        free(tmp->filhos);

        free(tmp); // Libera a memória da antiga raiz
    }
    return root;
}
