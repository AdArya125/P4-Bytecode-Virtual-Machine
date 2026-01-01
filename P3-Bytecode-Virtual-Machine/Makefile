# Compiler
CC = gcc

# Compiler flags
# -Wall -Wextra : catch common mistakes
# -g            : enable debugging (gdb, stack traces)
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = src/main.c src/vm.c

# Output binary name
TARGET = vm

# Default target
all: $(TARGET)

# Build the VM
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)
