CC = gcc
CFLAGS = -Wall -Wextra -Isrc
BIN_DIR = bin
SRC_DIR = src

TARGETS = $(BIN_DIR)/main $(BIN_DIR)/solver

all: $(TARGETS)

$(BIN_DIR)/main: $(SRC_DIR)/main.c $(SRC_DIR)/config.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR)/solver: $(SRC_DIR)/solver.c $(SRC_DIR)/config.h | $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -f $(TARGETS)
	rmdir $(BIN_DIR) 2>/dev/null || true

test: all
	@echo "Testing solver..."
	$(BIN_DIR)/solver 1
	$(BIN_DIR)/solver 3
	@echo "Tests complete."

.PHONY: all clean test
