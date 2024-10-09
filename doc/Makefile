# Install
BIN = stddoc
IN  = ../jquick.h
OUT = index.html

# Flags
CFLAGS += -std=c99 -pedantic -O2

SRC = stddoc.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
endif

install: $(BIN)
	./$(BIN) < $(IN) > $(OUT)

$(BIN): clean
	$(CC) $(SRC) $(CFLAGS) -o $(BIN)

clean:
	rm -f $(BIN) $(OBJS)
