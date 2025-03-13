#include "arvoreB.h"

int main(int argc, char* argv[]){

    BT* bt = criaBT();

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen("saida.txt", "w");

    char linha[50];

    int ordem, num_operacoes;
    fscanf(input, "%d\n", &ordem);
    fscanf(input, "%d\n", &num_operacoes);
    fgetc(input);

    for(int i=0; i<num_operacoes; i++){
        fgets(linha, sizeof(linha), input);

        char operacao;
        int n1, n2;
        if (sscanf(linha, " %c %d, %d", &operacao, &n1, &n2) == 3) {
            if (operacao == 'I') {
                // inserir(n1, n2);
                // printf("Inseri\n");
            }
        } else if (sscanf(linha, " %c %d", &operacao, &n1) == 2) {
            if (operacao == 'R') {
                // printf("Removi\n");
                // remover(n1);
            } else if (operacao == 'B') {
                // printf("Busquei\n");
                // buscar(n1);
            }
        }
    }

    // imprime estado final

    fclose(input);
    return 0;
}