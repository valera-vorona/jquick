BIN = test
DIR = bin
MKDIR = mkdir -p

# Flags
CFLAGS += -std=c90 -pedantic -O2

SRC = test.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
endif

$(BIN): clean
	$(MKDIR) $(DIR)/
	$(CC) $(SRC) $(CFLAGS) -o $(DIR)/$(BIN)
#	$(CXX) $(SRC) -o $(DIR)/$(BIN)

clean:
	rm -f $(OBJS)
	rm -rf $(DIR)
