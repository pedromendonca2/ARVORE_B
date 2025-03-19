#!/bin/bash

# Para o script funcionar deve existir o diretório casos_teste_v5 com os arquivos de teste de entrada
# Serão criados os arquivos de saída no diretório saidas/
# O script compila o código e executa os testes
# Uso: ./testador.sh [valgrind]
# Pode escolher usar o valgrind

# Variáveis
TEST_DIR="caso_teste_v5"
OUTPUT_DIR="saidas"
BIN="./trab2"
VALGRIND="valgrind"
NUM_RUNS=1  # Número de execuções para calcular a média

# Função para calcular a média dos tempos de execução
calculate_average() {
    local total=0
    local count=0
    for time in "$@"; do
        total=$(echo "$total + $time" | bc)
        count=$((count + 1))
    done
    echo "scale=6; $total / $count" | bc
}

# Função para executar testes com trab1 (Heap)
test() {
    for test in $TEST_DIR/*.txt; do
        output=$OUTPUT_DIR/saida_$(basename $test)
        times=()
        for i in $(seq 1 $NUM_RUNS); do
            if [ "$USE_VALGRIND" == "true" ]; then
                valgrind_output=$OUTPUT_DIR/valgrind_$(basename $test .txt).log
                $VALGRIND --log-file=$valgrind_output $BIN $test $output
            else
                start_time=$(date +%s.%N)
                $BIN $test $output
                end_time=$(date +%s.%N)
                elapsed_time=$(echo "$end_time - $start_time" | bc)
                times+=($elapsed_time)
            fi
            sleep 1  # Aguarda 1 segundo para que a memória seja liberada antes de executar o próximo teste
        done
        if [ "$USE_VALGRIND" != "true" ]; then
            average_time=$(calculate_average "${times[@]}")
            echo "$(basename $test): Tempo médio de execução: $average_time segundos"
        fi
    done
}
# Verifica os argumentos passados para o script
make clean
make
if [ "$1" == "valgrind" ]; then
        USE_VALGRIND="true"
    else
        USE_VALGRIND="false"
fi

test