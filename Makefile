# Compiler
CC=gcc

# Compiler flags
CFLAGS= -g -I./include

# Directories
ODIR=obj
SRCDIR=src
INCDIR=include

# Libraries
LIBS=-lm

# Dependencies
_DEPS = translate.h prints.h dump.h patterns.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = main.o translate.o prints.o dump.o patterns.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Targets
all: disassembler

disassembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f $(SRCDIR)/main
	rm -f disassembler
