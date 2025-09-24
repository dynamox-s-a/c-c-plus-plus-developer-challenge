# Compiler
CC = gcc

# Include
INCLUDES = \
	-IDrivers/Arithmetic/Inc \
	-IDrivers/Cli_Handler/Inc \
	-IDrivers/Logs_File/Inc \
	-IDrivers/Misc/Inc \
	-IDrivers/Unity/Inc \
	-IDrivers/Arithmetic/Test/Inc \
	-IDrivers/Cli_Handler/Test/Inc \
	-IDrivers/Logs_File/Test/Inc \
	-IDrivers/Misc/Test/Inc

# Flags
CFLAGS = -Wall -Wextra -g $(INCLUDES)

# Files .C
SRC = \
    $(wildcard Src/*.c) \
	$(wildcard Drivers/Arithmetic/Src/*.c) \
    $(wildcard Drivers/Cli_Handler/Src/*.c) \
	$(wildcard Drivers/Logs_File/Src/*.c)\
    $(wildcard Drivers/Misc/Src/*.c)\
	$(wildcard Drivers/Unity/Src/*.c)\
	$(wildcard Drivers/Arithmetic/Test/Src/*.c) \
	$(wildcard Drivers/Cli_Handler/Test/Src/*.c) \
	$(wildcard Drivers/Logs_File/Test/Src/*.c) \
	$(wildcard Drivers/Misc/Test/Src/*.c)

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
