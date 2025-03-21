#include "arvoreB.h"

#define MAX_LINHA 50

int main(int argc, char* argv[]){
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo de entrada> <arquivo de saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE* output = fopen(argv[2], "w");
    if (output == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(input);
        return 1;
    }

    char linha[MAX_LINHA];

    int ordem, num_operacoes;
    fscanf(input, "%d", &ordem);
    fscanf(input, "%d", &num_operacoes);
    fgetc(input);

    BT* bt = criaBT(ordem);

    for(int i=0; i<num_operacoes; i++){
        fgets(linha, sizeof(linha), input);
        char *ptr = linha;
        while (*ptr == ' ') ptr++;

        char operacao;
        int n1=0, n2=0;

        if (sscanf(linha, "%c %d , %d", &operacao, &n1, &n2) == 3) {
            //printf("%c %d %d\n", operacao, n1, n2);
            if (operacao == 'I') {
                //printf("Inseri\n");
                insere(bt, n1, n2);
                //printf("Inseri_final\n");
            }
        } else if (sscanf(linha, "%c %d", &operacao, &n1) == 2) {
            ////printf("Entrei aqui\n");
            if (operacao == 'R') {
                // //printf("Removi\n");
                // remover(n1);
            } else if (operacao == 'B') {
                //printf("Busquei\n");
                busca(output, bt, n1);
                //printf("Busquei_final\n");
            }
        } else{
            //printf("Operação inválida\n");
            //printf("%s\n", linha);
            exit(1);
        }
    }

    //printf("Imprimi\n");
    imprime(output, bt);
    //printf("Imprimi_final\n");

    destroiBT(bt);
    fclose(input);
    fclose(output);
    return 0;
}