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

# Common source files (without main.c and test.c)
SRC = \
    $(wildcard Src/*.c) \
	$(wildcard Drivers/Arithmetic/Src/*.c) \
    $(wildcard Drivers/Cli_Handler/Src/*.c) \
	$(wildcard Drivers/Logs_File/Src/*.c) \
    $(wildcard Drivers/Misc/Src/*.c) \
	$(wildcard Drivers/Unity/Src/*.c) \
	$(wildcard Drivers/Arithmetic/Test/Src/*.c) \
	$(wildcard Drivers/Cli_Handler/Test/Src/*.c) \
	$(wildcard Drivers/Logs_File/Test/Src/*.c) \
	$(wildcard Drivers/Misc/Test/Src/*.c)

# Remove main.c and test.c from common sources
SRC := $(filter-out Src/main.c Src/test.c, $(SRC))

# Objects (.o)
OBJ = $(SRC:.c=.o)

# Executable files
TARGET_MAIN = main.exe
TARGET_TEST = test.exe

# Default Rule (build both executables)
all: $(TARGET_MAIN) $(TARGET_TEST)

# How to generate main executable
$(TARGET_MAIN): $(OBJ) Src/main.o
	$(CC) $(OBJ) Src/main.o -o $@

# How to generate test executable
$(TARGET_TEST): $(OBJ) Src/test.o
	$(CC) $(OBJ) Src/test.o -o $@

# How to generate .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run unit tests (compile and execute test.exe)
run_tests: $(TARGET_TEST)
	./$(TARGET_TEST)

# Clean generated files
clean:
	del /Q $(OBJ) Src\\main.o Src\\test.o $(TARGET_MAIN) $(TARGET_TEST)
