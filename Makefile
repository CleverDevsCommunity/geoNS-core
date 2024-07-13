# geons-core
NAME=geons-core
CC=gcc
SRC_DIR=./src
OUTPUT_DIR=./build
OUTPUT_FILE=$(OUTPUT_DIR)/$(NAME)
FILES=$(SRC_DIR)/main.c
LIBS=$(wildcard ${SRC_DIR}/libs/**/*.c)
FLAGS=-Ilibs -Iabstraction $(shell pkg-config --libs --cflags sqlite3 openssl)


clean:
	rm -rf $(OUTPUT_DIR)

create-output-dir: clean
	mkdir $(OUTPUT_DIR)

build: create-output-dir
	$(CC) -o $(OUTPUT_FILE) $(FILES) $(LIBS) $(FLAGS)