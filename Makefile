SHELL := cmd.exe

MINGW_BIN ?= C:/MinGW/bin
CC := $(MINGW_BIN)/gcc.exe

TARGET := output/main.exe

SRC := $(wildcard src/*.c) main.c
OBJ := $(patsubst %.c,build/%.o,$(notdir $(SRC)))

CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g3
CPPFLAGS := -Iinclude

.PHONY: all clean run dirs

all: dirs $(TARGET)

dirs:
	if not exist build mkdir build
	if not exist output mkdir output

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

build/main.o: main.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

run: all
	$(TARGET)

clean:
	if exist build rmdir /s /q build
	if exist output rmdir /s /q output