# Thomas Daley
# September 13, 2021

# A generic build template for C/C++ programs

# executable name
EXE = dracon

# C compiler
CC = gcc
# linker
LD = gcc

# C flags
CFLAGS = -Wall -g
# dependency-generation flags
DEPFLAGS = -MMD -MP
# linker flags
LDFLAGS = 
# library flags
LDLIBS = 

# build directories
BIN = .
OBJ = out
SRC = src

INCLUDE = -Isrc/

SOURCES := $(shell find $(SRC) -name '*.c')
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))
OBJDIRS := $(sort $(dir $(OBJECTS)))

# include compiler-generated dependency rules
DEPENDS := $(OBJECTS:.o=.d)

# compile C source
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(INCLUDE) -c -o $@
# link objects
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) $(OBJECTS) -o $@

.DEFAULT_GOAL = all

.PHONY: all
all: $(BIN)/$(EXE)

$(BIN)/$(EXE): $(SRC) $(OBJ) $(BIN) $(OBJECTS)
	$(LINK.o)

$(SRC):
	mkdir -p $(SRC)

$(OBJDIRS):
	mkdir -p $@

$(OBJ): | $(OBJDIRS)
	mkdir -p $(OBJ)

$(BIN):
	mkdir -p $(BIN)

$(OBJ)/%.o:	$(SRC)/%.c
	$(COMPILE.c) $<

# force rebuild
.PHONY: remake
remake:	clean $(BIN)/$(EXE)

# execute the program
.PHONY: run
run: $(BIN)/$(EXE)
	./$(BIN)/$(EXE)

# remove previous build and objects
.PHONY: clean
clean:
	$(RM) $(OBJECTS)
	$(RM) $(DEPENDS)
	$(RM) $(BIN)/$(EXE)

# remove everything except source
.PHONY: reset
reset:
	$(RM) -r $(OBJ)
	$(RM) -r $(BIN)

-include $(DEPENDS)
