# Compiler
CC = gcc

# Include
INCLUDES = -IDrivers/Cli_Handler/Inc

# Flags
CFLAGS = -Wall -Wextra -g $(INCLUDES)

# Files .C
SRC = $(wildcard Src/*.c Drivers/Cli_Handler/Src/*.c)

# Objects (.o)
OBJ = $(SRC:.c=.o)

# Executable file
TARGET = main.exe

# Default Rule
all: $(TARGET)

# How do generate executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@

# How do generate .o
src/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean generated files
clean:
	del /Q $(O
