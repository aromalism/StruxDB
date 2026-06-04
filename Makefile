CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
SRC = src/regndlog.c src/dbms.c
BIN = bin/strux

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN)
