# Makefile - Compilação do projeto

# Compilador
CC = gcc

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

# Arquivos-fonte e objeto
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Nome do executável final
TARGET = $(BUILD_DIR)/programa

# Flags de compilação
CFLAGS = -Wall -I$(INCLUDE_DIR)

# Regra padrão
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpar arquivos compilados
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

# Executar o programa
run: all
	./$(TARGET)
