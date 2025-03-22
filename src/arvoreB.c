#include "arvoreB.h"
#include <math.h>

// Estrutura para manipular o arquivo binário
typedef struct {
    int posicaoRoot;
    int posicaoLivre;
    FILE *arqBinario;
} bin;

// Função para calcular o offset no arquivo binário
int calculate_offset(int diskID, int order) {
    return diskID * (sizeof(int) * (order - 1) + sizeof(int) * order + sizeof(int) * 3);
}

// Função para criar um arquivo binário e colocar o ponteiro na struct bin
bin *create_bin() {
    bin *b = calloc(1, sizeof(bin));
    FILE *fp = fopen("arvoreB.bin", "wb+"); // "wb+" permite leitura e escrita
    if (!fp) {
        perror("Erro ao criar arquivo binário");
        exit(EXIT_FAILURE);
    }
    b->arqBinario = fp;
    b->posicaoRoot = -1;
    b->posicaoLivre = 0; // Primeiro nó após o cabeçalho
    return b;
}

// Função para fechar o arquivo binário
void close_bin(bin* bin) {
    fclose(bin->arqBinario);
    free(bin);
}

// Função para calcular a próxima posição livre no arquivo binário
void calculaProximaPosicaoLivre(bin *bin, int order) {
    bin->posicaoLivre += (sizeof(int) * (order - 1) + sizeof(int) * order + sizeof(int) * 3);
}

int diskSearch(FILE* fp, int ordem, int chave, int posicaoRoot) {
    // Lê o nó atual do arquivo binário usando disk_read
    Node* node = disk_read(posicaoRoot, ordem, fp, NULL);

    // Busca a chave no nó atual
    int i = 0;
    while (i < getNumKeys(node) && chave > getKeysValues(node, i)) {
        i++;
    }

    // Se a chave for encontrada, retorna o deslocamento do nó
    if (i < getNumKeys(node) && chave == getKeysValues(node, i)) {
        int posInDisk = getPosInDisk(node); // Usa o getter para obter o deslocamento
        destroiNode(node); // Libera a memória do nó lido
        return posInDisk;
    }

    // Se o nó for folha, a chave não está na árvore
    if (getIsLeaf(node)) { // Usa o getter para verificar se é folha
        destroiNode(node); // Libera a memória do nó lido
        return -1; // Chave não encontrada
    }

    // Caso contrário, desce para o filho apropriado
    int childOffset = getKidsValues(node, i); // Usa o getter para obter o deslocamento do filho
    destroiNode(node); // Libera a memória do nó lido

    if (childOffset == -1) {
        return -1; // Chave não encontrada
    }

    // Chama recursivamente a função para o filho
    return diskSearch(fp, ordem, chave, childOffset);
}
// Função para ler um nó da árvore B de um arquivo binário
Node *disk_read(int diskID, int order, FILE *fp, bin *bin) {
    int offset = calculate_offset(diskID, order);
    fseek(fp, offset, SEEK_SET); // Posiciona o ponteiro do arquivo no offset correto

    int numKeys, isLeaf, pos_in_disk;

    if (fread(&numKeys, sizeof(int), 1, fp) != 1 ||
        fread(&isLeaf, sizeof(int), 1, fp) != 1 ||
        fread(&pos_in_disk, sizeof(int), 1, fp) != 1) {
        perror("Erro ao ler do arquivo");
        exit(EXIT_FAILURE);
    }

    Node *nodeLido = readNode(numKeys, isLeaf, order, bin);

    int *keys = (int *)calloc(order - 1, sizeof(int));
    int *kids = (int *)malloc(order * sizeof(int));

    if (!keys || !kids) {
        perror("Falha na alocação de memória");
        exit(EXIT_FAILURE);
    }

    if (fread(keys, sizeof(int), order - 1, fp) != order - 1 ||
        fread(kids, sizeof(int), order, fp) != order) {
        perror("Erro ao ler chaves ou filhos do arquivo");
        exit(EXIT_FAILURE);
    }

    setPosInDisk(nodeLido, pos_in_disk);

    for (int i = 0; i < order - 1; i++)
        setKeysValues(nodeLido, keys[i], i);
    for (int i = 0; i < order; i++)
        setKidsValues(nodeLido, kids[i], i);

    free(keys);
    free(kids);

    return nodeLido;
}

// Função para criar um nó da árvore B a partir de um arquivo binário
Node *readNode(int numKeys, int isLeaf, int order, bin *bin) {
    Node *node = createNode(order, isLeaf);
    setNumKeys(node, numKeys);
    return node;
}

// Função para escrever um nó da árvore B em um arquivo binário
void disk_write(Node *node, int order, FILE *fp) {
    int offset = calculate_offset(getPosInDisk(node), order);
    fseek(fp, offset, SEEK_SET); // Posiciona o ponteiro do arquivo no offset correto

    int numKeys = getNumKeys(node);
    int isLeaf = getIsLeaf(node);
    int posInDisk = getPosInDisk(node);

    if (fwrite(&numKeys, sizeof(int), 1, fp) != 1 ||
        fwrite(&isLeaf, sizeof(int), 1, fp) != 1 ||
        fwrite(&posInDisk, sizeof(int), 1, fp) != 1) {
        perror("Erro ao escrever no arquivo");
        exit(EXIT_FAILURE);
    }

    if (fwrite(getKeys(node), sizeof(int), order - 1, fp) != order - 1 ||
        fwrite(getKids(node), sizeof(int), order, fp) != order) {
        perror("Erro ao escrever chaves ou filhos no arquivo");
        exit(EXIT_FAILURE);
    }
}

struct arvore{
    int ordem;
    int num_nodes;
    Node* raiz;
};

struct node{
    int num_chaves; // p/ nós internos e folhas, o min. de chaves é (t/2) − 1 e o max. é ́t − 1, já p/ a raíz, o min. de chaves é 1 e o max. é t − 1
    bool ehFolha;
    int deslocamento; // A posição do nó  no arquivo binário (o deslocamento em bytes para acessar/atualizar este nó dentro do arquivo binario)
    int* chaves; // Este número está no intervalo [⌈t/2⌉−1, t−1] para nós internos e folhas, e entre [1,t−1] para a raiz.
    int* registros; // cada um indexado por uma chave 
    Node** filhos; // (sempre igual ao número de chaves armazenadas + 1)
};

// Getters e Setters
int getNumKeys(Node* node) {
    return node->num_chaves;
}

int getIsLeaf(Node* node) {
    return node->ehFolha;
}

int getPosInDisk(Node* node) {
    return node->deslocamento;
}

int* getKeys(Node* node) {
    return node->chaves;
}

Node** getKids(Node* node) {
    return node->filhos;
}

int getKeysValues(Node* node, int index) {
    return node->chaves[index];
}

int getKidsValues(Node* node, int index) {
    return node->filhos[index]->deslocamento;
}

void setKeysValues(Node* node, int value, int index) {
    node->chaves[index] = value;
}

void setKidsValues(Node* node, int deslocamento, int index) {
    // Cria um novo nó filho com o deslocamento especificado
    Node* filho = malloc(sizeof(Node));
    filho->deslocamento = deslocamento;
    node->filhos[index] = filho;
}

void removeRec(Node* node, int k, int ordem);

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

void destroiBT(BT* x){
    if(x == NULL) return;

    destroiNode(x->raiz);
    free(x);
}

BT* criaBT(int ordem){
    BT* bt = malloc(sizeof(BT));
    if(bt == NULL){ 
        perror("Memory allocation failed"); 
        exit(1);
    }

    bt->ordem = ordem;
    bt->num_nodes = 0;
    bt->raiz = NULL;

    return bt;
}

Node* criaNode(BT* bt, bool ehFolha){
    Node* x = malloc(sizeof(Node));
    if(x == NULL){
        perror("Memory allocation failed");
        exit(1);
    }

    x->num_chaves = 0;
    x->ehFolha = ehFolha;

    x->chaves = malloc((bt->ordem) * sizeof(int));
    x->registros = malloc((bt->ordem) * sizeof(int));
    x->filhos = malloc((bt->ordem + 1) * sizeof(Node*));

    for(int i=0; i<bt->ordem + 1; i++){  // Corrigido: bt->ordem + 1
        x->filhos[i] = NULL;
    }

    //x->deslocamento = 0;

    return x;
}

// Função para dividir um nó cheio
void divideFilho(BT* bt, Node* pai, int k) {
    Node* filho = pai->filhos[k]; // Nó filho que está cheio
    Node* novoNode = criaNode(bt, filho->ehFolha); // Novo nó irmão (à direita)
    
    int meio = 0;
    if (bt->ordem % 2 == 0) { // Ordem par
        meio = bt->ordem / 2 - 1;
    } else { // Ordem ímpar
        meio = bt->ordem / 2;
    }
    
    // Calcula o número correto de chaves para o novo nó
    novoNode->num_chaves = filho->num_chaves - meio - 1;
    
    // Copia a metade superior das chaves e registros para o novo nó
    for (int i = 0; i < novoNode->num_chaves; i++) {
        novoNode->chaves[i] = filho->chaves[i + meio + 1];
        novoNode->registros[i] = filho->registros[i + meio + 1];
    }

    // Se o nó filho não for folha, copia também os ponteiros dos filhos
    if (!filho->ehFolha) {
        for (int i = 0; i <= novoNode->num_chaves; i++) {  // <= para copiar todos os ponteiros corretos
            novoNode->filhos[i] = filho->filhos[i + meio + 1];
        }
    }

    filho->num_chaves = meio; // Número de chaves restantes no nó original

    // Desloca os filhos do pai para abrir espaço para o novo nó
    for (int i = pai->num_chaves; i > k; i--) {
        pai->filhos[i + 1] = pai->filhos[i];
    }

    pai->filhos[k + 1] = novoNode; // Insere o novo nó como filho do pai

    // Desloca as chaves do pai para abrir espaço para a chave promovida
    for (int i = pai->num_chaves - 1; i >= k; i--) {
        pai->chaves[i + 1] = pai->chaves[i];
        pai->registros[i + 1] = pai->registros[i];
    }

    // Promove a chave do meio para o pai
    pai->chaves[k] = filho->chaves[meio];
    pai->registros[k] = filho->registros[meio];
    pai->num_chaves++;
}



// ... (código anterior: structs, criaNode, divideFilho - a divideFilho está correta agora) ...

// Função recursiva para inserir uma chave na árvore B
int insereNode(BT* bt, Node* node, int chave, int reg) {
    if (node == NULL) {
        // Árvore vazia, cria o primeiro nó (raiz)
        Node* novoNode = criaNode(bt, true);
        novoNode->chaves[0] = chave;
        novoNode->registros[0] = reg;
        novoNode->num_chaves = 1;
        return novoNode;
    }

    int i = node->num_chaves - 1;

    // Encontra a posição correta para inserir a chave ou descer na árvore
    while (i >= 0 && chave < node->chaves[i]) {
        i--;
    }

    if (i >= 0 && chave == node->chaves[i]) {
        // Chave já existe, atualiza o registro (sem inserção)
        node->registros[i] = reg;
        return node;
    }

    if (node->ehFolha) {
        // Inserção em nó folha

        // Desloca as chaves e registros para abrir espaço
        for (int j = node->num_chaves; j > i + 1; j--) {
            node->chaves[j] = node->chaves[j - 1];
            node->registros[j] = node->registros[j - 1];
        }

        // Insere a nova chave e registro
        node->chaves[i + 1] = chave;
        node->registros[i + 1] = reg;
        node->num_chaves++;

    } else {
        // Inserção em nó interno: Desce para o filho correto
        i++;
        node->filhos[i] = insereNode(bt, node->filhos[i], chave, reg);

        // Verifica se o filho precisa ser dividido
        if (node->filhos[i]->num_chaves == bt->ordem) {
            divideFilho(bt, node, i);
        }
    }
    
    return node; // Retorna o nó atual
}


// Função principal de inserção (a ser chamada pelo usuário)
void insere(BT* bt, int chave, int reg) {
    // Se a árvore estiver vazia
    if (bt->raiz == NULL) {
        bt->raiz = criaNode(bt, true);
        bt->raiz->chaves[0] = chave;
        bt->raiz->registros[0] = reg;
        bt->raiz->num_chaves = 1;
        return;
    }
    
    // Insere a chave na árvore
    bt->raiz = insereNode(bt, bt->raiz, chave, reg);
    
    // Verifica se a raiz precisa ser dividida
    if (bt->raiz->num_chaves == bt->ordem) {
        Node* novaRaiz = criaNode(bt, false);  // Nova raiz não é folha
        novaRaiz->filhos[0] = bt->raiz;
        divideFilho(bt, novaRaiz, 0);
        bt->raiz = novaRaiz; // Atualiza a raiz da árvore
    }
}

Node* buscaNode(FILE* f, Node* x, int k){
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
    
    return buscaNode(f, x->filhos[i], k);
}

void busca(FILE* f, BT* bt, int k){
    //Node* x = bt->raiz;
    buscaNode(f, bt->raiz, k);
}

void imprime(FILE* f, BT* bt){
    fprintf(f, "\n-- ARVORE B\n");

    Node* x = bt->raiz;
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

// Funções de Remoção 
int retornaNumChaves(Node* x){
    return x->num_chaves;
}

/**
 * @brief Função interna que encontra o predecessor de uma chave em um nó não folha.
 *
 * O predecessor é a maior chave na subárvore à esquerda da chave.
 *
 * @param node O nó não folha.
 * @param index O índice da chave cujo predecessor está sendo procurado.
 * @return A chave predecessora.
 */
int encontraPredecessor(Node* node, int index) {
    Node* current = node->filhos[index];
    while (!current->ehFolha) {
        current = current->filhos[current->num_chaves]; // Vai para o filho mais à direita
    }
    return current->chaves[current->num_chaves - 1]; // Retorna a chave mais à direita (maior)
}

/**
 * @brief Função interna que o sucessor de uma chave em um nó não folha.
 *
 * O sucessor é a menor chave na subárvore à direita da chave.
 *
 * @param node O nó não folha.
 * @param index O índice da chave cujo sucessor está sendo procurado.
 * @return A chave sucessora.
 */
int encontraSucessor(Node* node, int index) {
    Node* current = node->filhos[index + 1];
    while (!current->ehFolha) {
        current = current->filhos[0]; // Vai para o filho mais à esquerda
    }
    return current->chaves[0]; // Retorna a chave mais à esquerda (menor)
}

/**
 * @brief Função interna que mescla dois filhos de um nó.
 *
 * Esta função é chamada quando um nó filho tem menos que o número mínimo de chaves
 * durante a remoção. Ela mescla o filho com um de seus irmãos adjacentes.
 *
 * @param node O nó pai.
 * @param index O índice do filho que será mesclado com seu irmão à direita.
 * @param ordem A ordem da Árvore B.
 */
void mergeFilhos(Node* node, int index, int ordem) {
    Node* filhoEsquerda = node->filhos[index];
    Node* filhoDireita = node->filhos[index + 1];

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



/**
 * @brief Função interna que remove uma chave de um nó folha.
 *
 * Simplesmente desloca as chaves e registros subsequentes para a esquerda para
 * preencher o espaço da chave removida.
 *
 * @param node O nó folha.
 * @param index O índice da chave a ser removida.
 */
void removeDeFolha(Node* node, int index) {
    // Desloca todas as chaves e registros após o índice para a esquerda
    for (int i = index + 1; i < node->num_chaves; i++) {
        node->chaves[i - 1] = node->chaves[i];
        node->registros[i - 1] = node->registros[i];
    }
    node->num_chaves--; // Decrementa o número de chaves
}

/**
 * @brief Função interna que remove uma chave de um nó não folha.
 *
 * Esta função lida com os três casos de remoção em um nó não folha:
 *   1. Se o filho predecessor tiver chaves suficientes, substitui a chave a ser
 *      removida pelo seu predecessor e remove recursivamente o predecessor.
 *   2. Se o filho sucessor tiver chaves suficientes, substitui a chave a ser
 *      removida pelo seu sucessor e remove recursivamente o sucessor.
 *   3. Se nenhum dos filhos adjacentes tiver chaves suficientes, mescla-os e remove k do nó mesclado
 *
 * @param node O nó não folha.
 * @param index O índice da chave a ser removida.
 * @param ordem A ordem da arvore B.
 */
void removeDeNaoFolha(Node* node, int index, int ordem) {
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

/**
 * @brief Função interna que empresta uma chave do irmão anterior (esquerda).
 *
 * Move a chave mais à direita do irmão esquerdo para o pai e a chave do pai
 * para o filho que precisa de uma chave.  Ajusta os ponteiros dos filhos, se
 * necessário.
 *
 * @param node O nó pai.
 * @param index O índice do filho que precisa emprestar uma chave.
 */
void emprestaDoAnterior(Node* node, int index) {

    Node* filho = node->filhos[index];
    Node* irmao = node->filhos[index - 1];

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

/**
 * @brief Função interna que empresta uma chave do próximo irmão (direita).
 *
 * Move a chave mais à esquerda do irmão direito para o pai e a chave do pai
 * para o filho que precisa de uma chave. Ajusta os ponteiros dos filhos, se
 * necessário.
 *
 * @param node O nó pai.
 * @param index O índice do filho que precisa emprestar uma chave.
 */
void emprestaDoProximo(Node* node, int index) {

    Node* filho = node->filhos[index];
    Node* irmao = node->filhos[index + 1];

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

/**
 * @brief Função interna que garante que um nó filho tenha pelo menos o número mínimo de chaves
 *        após uma remoção.
 *
 * Se o filho tiver menos que o número mínimo de chaves, tenta emprestar uma chave
 * de um irmão adjacente. Se nenhum irmão tiver chaves suficientes para emprestar,
 * mescla o filho com um irmão.
 *
 * @param node O nó pai.
 * @param index O índice do filho que precisa ter chaves suficientes.
 * @param ordem A ordem da arvore B
 */
void preenche(Node* node, int index, int ordem) {
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



/**
 * @brief Função interna recursiva principal para remover uma chave da Árvore B.
 *
 * Encontra a chave a ser removida e chama as funções auxiliares apropriadas
 * para realizar a remoção, dependendo se a chave está em um nó folha ou não folha,
 * e se os filhos têm o número mínimo de chaves.
 *
 * @param node O nó atual sendo examinado.
 * @param k A chave a ser removida.
 * @param ordem A ordem da Árvore B.
 */
void removeRec(Node* node, int k, int ordem) {
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
Node* removeKey(Node* root, int k, int ordem) {
    
    printf("Começando a remoção\n");
    if (!root) {
        printf("A árvore está vazia.\n");
        return root;
    }

    removeRec(root, k, ordem);

    // Se a raiz tiver 0 chaves após a remoção, torna o primeiro filho a nova raiz (se existir)
    if (root->num_chaves == 0) {
        Node* tmp = root;
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

int encontrar_posicao(struct node *no, int chave) {
    int i = 0;
    while (i < no->num_chaves && chave > no->chaves[i]) {
        i++;
    }
    return i;
}

void insereBinario(int chave, int reg, int ordem, FILE* fp, bin *bin) {
    if (!fp || !bin) {
        printf("Erro: arquivo ou estrutura binária inválida.\n");
        return;
    }

    // Lê a posição da raiz do início do arquivo binário
    Node* root = disk_read(bin->posicaoRoot, ordem, fp, bin);

    if (!root) {
        // Árvore vazia, cria o primeiro nó
        Node* novoNode = criaNode(ordem, 1);
        setKeysValues(novoNode, chave, 0);
        setNumKeys(novoNode, 1);
        setPosInDisk(novoNode, bin->posicaoLivre);

        // Atualiza a posição da raiz
        bin->posicaoRoot = bin->posicaoLivre;
        bin->posicaoLivre += (sizeof(int) * (ordem - 1) + sizeof(int) * ordem + sizeof(int) * 3);

        // Escreve o novo nó no arquivo binário
        disk_write(novoNode, ordem, fp);
        destroiNode(novoNode);
    } else {
        // Insere na árvore existente
        Node* novaRaiz = insereNodeBinario(root, chave, reg, ordem, fp, bin);

        // Se houve mudança na raiz, atualiza no arquivo binário usando `disk_write`
        if (getPosInDisk(novaRaiz) != bin->posicaoRoot) {
            bin->posicaoRoot = getPosInDisk(novaRaiz);
            disk_write(novaRaiz, ordem, fp);
        }

        destroiNode(root);
        if (novaRaiz != root) {
            destroiNode(novaRaiz);
        }
    }

    // Sempre atualiza a raiz no arquivo binário
    disk_write(root, ordem, fp);
}


// Função auxiliar modificada para trabalhar com arquivo
Node* insereNodeBinario(Node* node, int chave, int reg, int ordem, FILE* fp, bin* bin) {
    if (node == NULL) {
        Node* novoNode = criaNode(ordem, 1);
        novoNode->chaves[0] = chave;
        novoNode->registros[0] = reg;
        novoNode->num_chaves = 1;
        
        // Calcula nova posição no arquivo
        fseek(fp, 0, SEEK_END);
        novoNode->deslocamento = ftell(fp);
        
        disk_write(novoNode, ordem, fp);
        return novoNode;
    }

    // ...resto do código de inserção existente, mas adicionando disk_write após cada modificação...

    if (node->ehFolha) {
        // ...código de inserção em folha...
        disk_write(node, ordem, fp);
    } else {
        // ...código de inserção em nó interno...
        Node* filho = disk_read(node->filhos[i]->deslocamento, ordem, fp, bin);
        Node* novoFilho = insereNodeBinario(filho, chave, reg, ordem, fp, bin);
        node->filhos[i] = novoFilho;
        
        if (novoFilho->num_chaves == ordem) {
            divideFilhoBinario(node, i, ordem, fp, bin);
        }
        
        disk_write(node, ordem, fp);
    }
    
    return node;
}

Node* insereNode2Binario(Node* node, int chave, int reg, int ordem, FILE* fp, bin* bin) {
    int i = node->num_chaves - 1;

    if (node->ehFolha) {
        // Move as chaves maiores para abrir espaço
        while (i >= 0 && chave < node->chaves[i]) {
            node->chaves[i + 1] = node->chaves[i];
            node->registros[i + 1] = node->registros[i];
            i--;
        }
        
        // Insere a nova chave
        node->chaves[i + 1] = chave;
        node->registros[i + 1] = reg;
        node->num_chaves++;
        
        disk_write(node, ordem, fp);
    } else {
        // Encontra o filho correto
        while (i >= 0 && chave < node->chaves[i]) {
            i--;
        }
        i++;
        
        Node* filho = disk_read(node->filhos[i]->deslocamento, ordem, fp, bin);
        
        if (filho->num_chaves == ordem - 1) {
            // Filho está cheio, precisa dividir
            divideFilhoBinario(node, i, ordem, fp, bin);
            if (chave > node->chaves[i]) {
                i++;
            }
            filho = disk_read(node->filhos[i]->deslocamento, ordem, fp, bin);
        }
        
        insereNodeBinario(filho, chave, reg, ordem, fp, bin);
    }
    
    return node;
}

void divideFilhoBinario(Node* pai, int i, int ordem, FILE* fp, bin* bin) {
    Node* filho = disk_read(pai->filhos[i]->deslocamento, ordem, fp, bin);
    Node* novoNode = criaNode(ordem, filho->ehFolha);
    
    // Calcula nova posição no arquivo para o novo nó
    fseek(fp, 0, SEEK_END);
    novoNode->deslocamento = ftell(fp);
    
    int t = ordem / 2;
    novoNode->num_chaves = t - 1;
    
    // Copia as chaves maiores para o novo nó
    for (int j = 0; j < t - 1; j++) {
        novoNode->chaves[j] = filho->chaves[j + t];
        novoNode->registros[j] = filho->registros[j + t];
    }
    
    // Se não for folha, copia também os ponteiros dos filhos
    if (!filho->ehFolha) {
        for (int j = 0; j < t; j++) {
            novoNode->filhos[j] = filho->filhos[j + t];
        }
    }
    
    filho->num_chaves = t - 1;
    
    // Move os filhos do pai para abrir espaço para o novo filho
    for (int j = pai->num_chaves; j >= i + 1; j--) {
        pai->filhos[j + 1] = pai->filhos[j];
    }
    
    pai->filhos[i + 1] = novoNode;
    
    // Move as chaves do pai
    for (int j = pai->num_chaves - 1; j >= i; j--) {
        pai->chaves[j + 1] = pai->chaves[j];
        pai->registros[j + 1] = pai->registros[j];
    }
    
    pai->chaves[i] = filho->chaves[t - 1];
    pai->registros[i] = filho->registros[t - 1];
    pai->num_chaves++;
    
    // Escreve todos os nós modificados no disco
    disk_write(filho, ordem, fp);
    disk_write(novoNode, ordem, fp);
    disk_write(pai, ordem, fp);
}

