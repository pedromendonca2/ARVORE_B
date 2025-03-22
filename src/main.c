#include "arvoreB.h"
#define MAX_LINHA 50
#define ARQUIVO_TMP "arvoreB.bin"
int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <arquivo de entrada> <arquivo de saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    FILE* binFile = fopen(ARQUIVO_TMP, "w+b");

    if (!input || !output || !binFile) {
        perror("Erro ao abrir arquivos");
        return 1;
    }

    bin *bin = create_bin(); 
    int ordem, num_operacoes;
    fscanf(input, "%d %d", &ordem, &num_operacoes);
    fgetc(input); // Consume newline

    // Initialize binary file with root offset
    int rootOffset = -1;
    fwrite(&rootOffset, sizeof(int), 1, binFile);

    char linha[MAX_LINHA];
    for(int i = 0; i < num_operacoes; i++) {
        fgets(linha, sizeof(linha), input);
        char operacao;
        int n1 = 0, n2 = 0;

        if (sscanf(linha, "%c %d , %d", &operacao, &n1, &n2) == 3) {
            if (operacao == 'I') {
                insereBinario(n1, n2, ordem, binFile, bin);
            }
        } else if (sscanf(linha, "%c %d", &operacao, &n1) == 2) {
            if (operacao == 'B') {
                diskSearch(binFile, ordem, n1, getposicaoRoot(bin));
                } else if (operacao == 'R') {
                remove_key(n1, ordem, binFile);
            }
        }
    }

    // Print final tree state
    imprime_arvore(output, binFile, ordem);

    fclose(input);
    fclose(output);
    fclose(binFile);
    remove(ARQUIVO_TMP); // Clean up temporary binary file
    return 0;
}