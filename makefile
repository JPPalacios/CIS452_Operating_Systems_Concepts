CC=gcc
CFLAGS=-Wall -Wextra -g

SRC_DIR=src
BIN_DIR=bin

SOURCES=$(wildcard $(SRC_DIR)/*.c)
OBJECTS=$(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SOURCES))

all: write read

write: $(BIN_DIR)/write

read: $(BIN_DIR)/read

$(BIN_DIR)/write: $(BIN_DIR)/write.o $(BIN_DIR)/common.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/read: $(BIN_DIR)/read.o $(BIN_DIR)/common.o
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJECTS): | $(BIN_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

.PHONY: all clean

clean:
	rm -rf $(BIN_DIR)
