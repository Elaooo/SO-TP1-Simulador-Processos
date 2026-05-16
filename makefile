# ==============================
# Compilador e flags
# ==============================
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

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
	$(SRC_DIR)/TAD_LeituraArquivo.c \
	$(SRC_DIR)/TAD_processo_simulado.c\
	$(SRC_DIR)/TAD_CPU.c\
	$(SRC_DIR)/fila.c\
	$(SRC_DIR)/tempo.c\
	$(SRC_DIR)/TAD_GerenciadorProcesso.c\
	$(SRC_DIR)/tabela_processos.c\
	$(SRC_DIR)/ProcessoControle.c\
	$(SRC_DIR)/processoImpressao.c\
	$(SRC_DIR)/globais.c

	
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