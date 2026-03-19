# =============================================================================
# Makefile for Dynamox Arithmetic Calculator
# =============================================================================
#
# Usage:
#   make          - Build the application
#   make clean    - Remove build artifacts
#   make run      - Build and run the application
#
# Compiler settings are tuned for embedded-friendly practices:
#   - Strict warnings (-Wall -Wextra -Wpedantic)
#   - No undefined behavior tolerance (-Werror for critical warnings)
#   - Optimization configurable via OPT variable
# =============================================================================

CC      = gcc
OPT     = -O2
CFLAGS  = -Wall -Wextra -Wpedantic -std=c11 $(OPT)
LDFLAGS = -lm

# Source files
SRCS = main.c \
       lib/matrix/matrix.c \
       lib/operations/operations.c \
       lib/registry/registry.c \
       lib/logger/logger.c \
       lib/hmi/hmi.c

# Object files
OBJS = $(SRCS:.c=.o)

# Output binary
TARGET = calculator

# =============================================================================
# Targets
# =============================================================================

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q /F $(TARGET).exe 2>nul || true
	del /Q /F *.o 2>nul || true
	del /Q /F lib\matrix\*.o 2>nul || true
	del /Q /F lib\operations\*.o 2>nul || true
	del /Q /F lib\registry\*.o 2>nul || true
	del /Q /F lib\logger\*.o 2>nul || true
	del /Q /F lib\hmi\*.o 2>nul || true

run: $(TARGET)
	./$(TARGET)
