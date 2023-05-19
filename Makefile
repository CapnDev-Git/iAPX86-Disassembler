# Compiler
CC = gcc

# Compiler flags
CFLAGS = -g -I./include

# Directories
ODIR = obj
SRCDIR = src
INCDIR = include
TESTDIR = test

# Libraries
LIBS = -lm

# Dependencies
_DEPS = translate.h prints.h dump.h patterns.h
DEPS = $(patsubst %,$(INCDIR)/%,$(_DEPS))

_OBJ = main.o translate.o prints.o dump.o patterns.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

_TESTOBJ = utest.o prints.o
TESTOBJ = $(patsubst %,$(ODIR)/%,$(_TESTOBJ))

# Rules
$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: $(TESTDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Targets
all: disassembler $(TESTDIR)/utest

disassembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

$(TESTDIR)/utest: $(TESTOBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o
	rm -f disassembler
	rm -f $(TESTDIR)/utest
