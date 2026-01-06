# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Output directories
BIN_DIR = Assets/bin

# VM sources
VM_SRCS = src/main.c src/vm.c
VM_BIN  = $(BIN_DIR)/vm

# Assembler sources
ASM_SRCS = \
	src/Assembler/asm_main.c \
    src/Assembler/assembler.c \
    src/Assembler/lexer.c \
	src/Assembler/symbols.c

ASM_BIN = $(BIN_DIR)/assembler

# Default target
all: $(VM_BIN) $(ASM_BIN)

# Build VM
$(VM_BIN): $(VM_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(VM_SRCS) -o $(VM_BIN)

# Build assembler
$(ASM_BIN): $(ASM_SRCS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $(ASM_SRCS) -o $(ASM_BIN)

# Clean
clean:
	rm -f $(VM_BIN) $(ASM_BIN)
