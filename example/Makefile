BIN = example
BINPP = examplepp
DIR = bin
MKDIR = mkdir -p

# Flags
CFLAGS += -I..
CXXFLAGS += -I..

SRC = example.c
OBJ = $(SRC:.c=.o)

SRCPP = examplepp.cpp
OBJPP = $(SRCPP:.cpp=.o)

ifeq ($(OS),Windows_NT)
BIN := $(BIN).exe
BINPP := $(BINPP).exe
endif

all: $(BIN) $(BINPP)

$(BIN): clean
	$(MKDIR) $(DIR)/
	$(CC) $(SRC) $(CFLAGS) -o $(DIR)/$(BIN)

#$(BINPP): clean
#	$(MKDIR) $(DIR)/
#	$(CXX) $(SRCPP) $(CXXFLAGS) -o $(DIR)/$(BINPP)

clean:
	rm -f $(OBJ) $(OBJPP)
	rm -rf $(DIR)
