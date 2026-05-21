# ==============================
# Compilador e flags
# ==============================
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

# ==============================
# Diretórios
# ==============================
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# ==============================
# Nome do executável
# ==============================
TARGET = simulator

# ==============================
# Sistema operacional
# ==============================
ifeq ($(OS),Windows_NT)
	EXE = $(BIN_DIR)/$(TARGET).exe
	MKDIR = if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)" & if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"
	RM = if exist "$(OBJ_DIR)\*.o" del /q "$(OBJ_DIR)\*.o" & if exist "$(EXE)" del /q "$(EXE)"
	RUN = $(EXE)
else
	EXE = $(BIN_DIR)/$(TARGET)
	MKDIR = mkdir -p $(OBJ_DIR) $(BIN_DIR)
	RM = rm -f $(OBJ_DIR)/*.o $(EXE)
	RUN = ./$(EXE)
endif

# ==============================
# Arquivos fonte
# ==============================
SRCS = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/LeituraArquivo.c \
	$(SRC_DIR)/ProcessoSimulado.c\
	$(SRC_DIR)/Cpu.c\
	$(SRC_DIR)/Fila.c\
	$(SRC_DIR)/Tempo.c\
	$(SRC_DIR)/GerenciadorProcesso.c\
	$(SRC_DIR)/TabelaProcessos.c\
	$(SRC_DIR)/ProcessoControle.c\
	$(SRC_DIR)/ProcessoImpressao.c\
	$(SRC_DIR)/Globais.c\
	$(SRC_DIR)/Escalonador.c

	
# ==============================
# Objetos
# ==============================
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# ==============================
# Regra padrão
# ==============================
all: $(EXE)

# ==============================
# Linkagem
# ==============================
$(EXE): $(OBJS)
	$(MKDIR)
	$(CC) $(OBJS) -o $(EXE)
	@echo "✅ Compilação concluída: $(EXE)"

# ==============================
# Compilação .c -> .o
# ==============================
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(MKDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# ==============================
# Limpeza
# ==============================
clean:
	$(RM)
	@echo "🧹 Limpeza concluída."

# ==============================
# Executar
# ==============================
run: all
	$(RUN)

# ==============================
# Executar com entrada de teste
# ==============================
test: all
	$(RUN) < tests/test1.txt