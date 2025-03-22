# Variáveis de compilação
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm
VALGRIND = valgrind
SRC_DIR = src
OBJ_DIR = obj
TEST_DIR = casos_teste_v5
OUTPUT_DIR = saidas
BIN = trab2

# Lista de objetos
OBJ_bTree = $(OBJ_DIR)/main.o $(OBJ_DIR)/arvoreB.o $(OBJ_DIR)/fila.o

# Regra padrão: compila a versão desejada
all: bTree

# Compilar a versão bTree
bTree: $(OBJ_bTree)
	$(CC) $(CFLAGS) -o $(BIN) $^ $(LDFLAGS)

# Criar diretório de objetos, se necessário
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Regras de compilação dos objetos
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza
clean:
	rm -rf $(OBJ_DIR) $(BIN) $(OUTPUT_DIR)/*.txt

# PHONY targets
.PHONY: all runbTree valgrindbTree testbTree clean