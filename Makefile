# Compiler
CC=gcc

# Compiler flags
CFLAGS= -g -I./include

# Directories
ODIR=obj

# Libraries
LIBS=-lm

# Dependencies
_OBJ = main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Targets
disassembler: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f ./obj/*.o
	rm -f src/main
	rm -f ./disassembler

#END MAKEFILE
