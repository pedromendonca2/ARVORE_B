# Trabalho 2 - Arvore B

## Para fazer os testes
O testador.sh é um script feito para executar todos os casos de testes e gerar os arquivos de saida no diretorio saidas
./testador

Padrão de execução do executável
```bash
make
./trab2 entrada.txt saida.txt
```

Exemplo de execução
```bash
make
./trab2 caso_teste_v5/caso_teste_1.txt saidas/saida_caso_teste_1.txt
```
# Disposição do repositório
.
├── T2_2024_2_especificacao_v2.pdf
├── caso_teste_v5
│   ├── caso_teste_1.txt
│   ├── caso_teste_2.txt
│   ├── caso_teste_3.txt
│   └── caso_teste_4.txt
├── makefile
├── obj
│   ├── arvoreB.o
│   ├── fila.o
│   └── main.o
├── readme.md
├── saidas
│   ├── saida_caso_teste_1.txt
│   ├── saida_caso_teste_2.txt
│   ├── saida_caso_teste_3.txt
│   ├── saida_caso_teste_4.txt
│   ├── valgrind_caso_teste_1.log
│   ├── valgrind_caso_teste_2.log
│   ├── valgrind_caso_teste_3.log
│   └── valgrind_caso_teste_4.log
├── src
│   ├── arvoreB.c
│   ├── arvoreB.h
│   ├── fila.c
│   ├── fila.h
│   └── main.c
├── testador.sh
└── trab2
