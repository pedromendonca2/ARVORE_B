#ifndef ARVORE_B
#define ARVORE_B

#include "fila.h"

typedef struct arvore BT;          // Estrutura que representa a Árvore B
typedef struct node Node;          // Estrutura que representa um nó da Árvore B
typedef struct bin bin;

// Getters e Setters
int getNumKeys(Node* node);
int getIsLeaf(Node* node);
int getPosInDisk(Node* node);
int* getKeys(Node* node);
Node** getKids(Node* node);
int getKeysValues(Node* node, int index);
int getKidsValues(Node* node, int index);
void setKeysValues(Node* node, int value, int index);
void setKidsValues(Node* node, int deslocamento, int index);

// Funções para manipular a estrutura binária
int getposicaoRoot(bin* bin);
int getposicaoLivre(bin* bin);
FILE *getarqBinario(bin* bin);
bin* create_bin();
void close_bin(bin* bin);
void calculaProximaPosicaoLivre(bin* bin, int order);

// Funções para manipular nós da Árvore B
Node* criaNode(int ordem, int ehFolha);
void destroiNode(Node* x);
int retornaNumChaves(Node* x);

// Funções para manipular a Árvore B
BT* criaBT(int ordem);
void destroiBT(BT* x);
void insere(BT* bt, int chave, int reg);
Node* buscaNode(FILE* f, Node* x, int k);
void busca(FILE* f, BT* bt, int k);
void imprime(FILE* f, BT* bt);

// Funções para manipular o arquivo binário
Node* disk_read(int diskID, int order, FILE* fp, bin* bin);
void disk_write(Node* node, int order, FILE* fp);
int diskSearch(FILE* fp, int ordem, int chave, int posicaoRoot);

// Funções auxiliares para inserção
void divideFilho(BT* bt, Node* pai, int k);
void insereBinario(int chave, int reg, int ordem, FILE* fp, bin* bin);
void divideFilhoBinario(Node* pai, int i, int ordem, FILE* fp, bin* bin);
void insereNodeBinario(Node* node, int chave, int reg, int ordem, FILE* fp, bin* bin);

// Funções auxiliares para remoção
Node* removeKey(Node* root, int k, int ordem);
void removeRec(Node* node, int k, int ordem);
void removeDeFolha(Node* node, int index);
void removeDeNaoFolha(Node* node, int index, int ordem);
int encontraPredecessor(Node* node, int index);
int encontraSucessor(Node* node, int index);
void mergeFilhos(Node* node, int index, int ordem);
void emprestaDoAnterior(Node* node, int index);
void emprestaDoProximo(Node* node, int index);
void preenche(Node* node, int index, int ordem);

/**
 * @brief Destrói um nó da Árvore B e seus descendentes (libera a memória alocada).
 *
 * Libera recursivamente a memória alocada para os nós da árvore,
 * começando pelos filhos e, em seguida, liberando o próprio nó.
 *
 * @param x Um ponteiro para o nó da Árvore B a ser destruído.
 */
void destroiNode(Node* x);

/**
 * @brief Destrói uma Árvore B completa (libera a memória alocada).
 *
 * Chama a função `destroiNode` para liberar a memória de todos os nós da árvore,
 * começando pela raiz.
 *
 * @param x Um ponteiro para a Árvore B a ser destruída.
 */
void destroiBT(BT* x);

/**
 * @brief Cria uma Árvore B vazia.
 *
 * Uma Árvore B vazia é representada por um ponteiro NULL.
 *
 * @param ordem A ordem da árvore B (número máximo de filhos por nó).
 * @return Um ponteiro para a Árvore B vazia.
 */
BT* criaBT(int ordem);

/**
 * @brief Cria um novo nó da Árvore B.
 *
 * Aloca memória para o nó e seus vetores de chaves, registros e filhos.
 * Inicializa o número de chaves como 0 e define se o nó é folha ou não.
 *
 * @param bt Um ponteiro para a Árvore B à qual o nó pertence.
 * @param ehFolha Indica se o novo nó é uma folha (true) ou um nó interno (false).
 * @return Um ponteiro para o nó recém-criado.
 */
Node* criaNode(BT* bt, bool ehFolha);

/**
 * @brief Divide um nó filho cheio em dois nós.
 *
 * Esta função é chamada quando um nó filho fica cheio durante a inserção.
 * Ela cria um novo nó, copia a metade superior das chaves e filhos do nó cheio
 * para o novo nó, e promove a chave mediana para o nó pai.
 *
 * @param bt Um ponteiro para a Árvore B.
 * @param pai O nó pai do nó filho que será dividido.
 * @param k O índice do filho cheio no vetor de filhos do pai.
 */
void divideFilho(BT* bt, Node* pai, int k);

/**
 * @brief Insere uma chave e um registro em um nó que não está cheio.
 *
 * Esta função é chamada pela função `insere` quando o nó atual não está cheio.
 * Se o nó for uma folha, a chave é inserida diretamente. Caso contrário,
 * a função encontra o filho apropriado e chama recursivamente `insereNonFull`
 * nesse filho. Se o filho estiver cheio, ele é dividido antes da chamada recursiva.
 *
 * @param bt Um ponteiro para a Árvore B.
 * @param node O nó onde a chave será inserida (ou um de seus descendentes).
 * @param chave A chave a ser inserida.
 * @param reg O registro associado à chave.
 * @return Um ponteiro para o nó onde a chave foi inserida.
 */
Node* insereNode(BT* bt, Node* node, int chave, int reg);

/**
 * @brief Insere uma chave e um registro em uma Árvore B.
 *
 * Se a árvore estiver vazia, cria um novo nó raiz. Se a raiz estiver cheia,
 * cria uma nova raiz e divide a raiz antiga. Caso contrário, chama a função
 * `insereNonFull` para inserir a chave no nó apropriado.
 *
 * @param bt Um ponteiro para a Árvore B.
 * @param k A chave a ser inserida.
 * @param reg O registro associado à chave.
 */
void insere(BT* bt, int k, int reg);

/**
 * @brief Busca uma chave em um nó da Árvore B.
 *
 * Realiza uma busca recursiva no nó para encontrar a chave `k`.
 *
 * @param f Um ponteiro para o arquivo onde as mensagens de saída serão escritas.
 * @param x O nó atual da árvore sendo examinado.
 * @param k A chave a ser buscada.
 * @return Um ponteiro para o nó que contém a chave, ou NULL se a chave não for encontrada.
 */
Node* buscaNode(FILE* f, Node* x, int k);

/**
 * @brief Busca uma chave em uma Árvore B.
 *
 * Chama a função `buscaNode` para realizar a busca a partir da raiz da árvore.
 *
 * @param f Um ponteiro para o arquivo onde as mensagens de saída serão escritas.
 * @param bt Um ponteiro para a Árvore B.
 * @param k A chave a ser buscada.
 */
void busca(FILE* f, BT* bt, int k);

/**
 * @brief Imprime a Árvore B em ordem (nível a nível).
 *
 * Utiliza uma fila para realizar uma travessia em largura na árvore,
 * imprimindo as chaves de cada nó em um formato específico.
 *
 * @param f Um ponteiro para o arquivo onde a árvore será impressa.
 * @param x A raiz da Árvore B a ser impressa.
 */
void imprime(FILE* f, BT* x);

/**
 * @brief Retorna o número de chaves em um nó da Árvore B.
 *
 * @param x Um ponteiro para o nó.
 * @return O número de chaves no nó.
 */
int retornaNumChaves(Node* x);

/**
 * @brief Remove uma chave da Árvore B.
 *
 * Realiza a remoção da chave `k` da árvore, ajustando a estrutura para manter
 * as propriedades da Árvore B. Se a raiz ficar vazia após a remoção, ajusta a raiz.
 *
 * @param root A raiz da Árvore B.
 * @param k A chave a ser removida.
 * @param ordem A ordem da Árvore B.
 * @return A nova raiz da Árvore B (pode mudar se a raiz original ficar vazia).
 */
Node* removeKey(Node* root, int k, int ordem);

#endif