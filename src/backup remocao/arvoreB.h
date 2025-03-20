#ifndef ARVORE_B
#define ARVORE_B

#include "fila.h"

typedef struct arvore BT;

/**
 * @brief Destrói uma Árvore B (libera a memória alocada).
 *
 * Libera recursivamente a memória alocada para os nós da árvore,
 * começando pelos filhos e, em seguida, liberando o próprio nó.
 *
 * @param x Um ponteiro para o nó da Árvore B a ser destruído.
 */
void destroiBT(BT* x);

/**
 * @brief Cria uma Árvore B vazia.
 *
 * Uma Árvore B vazia é representada por um ponteiro NULL.
 * @param ordem A ordem da árvore B.
 * @return NULL, representando uma árvore vazia.
 */
BT* criaBT();

/**
 * @brief Cria um novo nó da Árvore B.
 *
 * Aloca memória para o nó e seus vetores de chaves, registros e filhos.
 * Inicializa o número de chaves como 0 e define se o nó é folha ou não.
 *
 * @param ehFolha Indica se o novo nó é uma folha (true) ou um nó interno (false).
 * @param ordem A ordem da árvore B (número máximo de filhos).
 * @return Um ponteiro para o nó recém-criado.
 */
BT* criaNode(bool ehFolha, int ordem);

/**
 * @brief Divide um nó filho cheio em dois nós.
 *
 * Esta função é chamada quando um nó filho fica cheio durante a inserção.
 * Ela cria um novo nó, copia a metade superior das chaves e filhos do nó cheio
 * para o novo nó, e promove a chave mediana para o nó pai.
 *
 * @param pai O nó pai do nó filho que será dividido.
 * @param k O índice do filho cheio no vetor de filhos do pai.
 */
void divideFilho(BT* pai, int k, int ordem);

/**
 * @brief Insere uma chave e um registro em um nó que não está cheio.
 *
 * Esta função é chamada pela função `insere` quando o nó atual não está cheio.
 * Se o nó for uma folha, a chave é inserida diretamente. Caso contrário,
 * a função encontra o filho apropriado e chama recursivamente `insereNonFull`
 * nesse filho.  Se o filho estiver cheio, ele é dividido antes da chamada recursiva.
 *
 * @param x O nó onde a chave será inserida (ou um de seus descendentes).
 * @param k A chave a ser inserida.
 * @param reg O registro associado à chave.
 */
void insereNonFull(BT* x, int k, int reg, int ordem);

/**
 * @brief Insere uma chave e um registro em uma Árvore B.
 *
 * Se a árvore estiver vazia, cria um novo nó raiz. Se a raiz estiver cheia,
 * cria uma nova raiz e divide a raiz antiga. Caso contrário, chama a função
 * `insereNonFull` para inserir a chave no nó apropriado.
 *
 * @param x A raiz da Árvore B (pode ser NULL se a árvore estiver vazia).
 * @param k A chave a ser inserida.
 * @param reg O registro associado à chave.
 * @param ordem A ordem da Árvore B.
 * @return Um ponteiro para a raiz da Árvore B (pode ter mudado se a raiz original estava cheia).
 */
BT* insere(BT* x, int k, int reg, int ordem);

/**
 * @brief Busca uma chave em uma Árvore B.
 *
 * Realiza uma busca recursiva na árvore para encontrar a chave `k`.
 *
 * @param f  Um ponteiro para o arquivo onde as mensagens de saída serão escritas.
 * @param x  O nó atual da árvore sendo examinado.
 * @param k  A chave a ser buscada.
 * @return Um ponteiro para o nó que contém a chave, ou NULL se a chave não for encontrada.
 */
BT* busca(FILE* f, BT* x, int k);

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
int retornaNumChaves(BT* x);

/**
 * @brief Função pública para remover uma chave da Árvore B.
 *
 * Chama a função recursiva `removeRec` para realizar a remoção.
 * Se a raiz ficar vazia após a remoção, ajusta a raiz da árvore.
 *
 * @param root A raiz da Árvore B.
 * @param k A chave a ser removida.
 * @param ordem A ordem da Árvore B.
 *
 * @return A nova raiz da arvore B
 */
BT* removeKey(BT* root, int k, int ordem);
#endif