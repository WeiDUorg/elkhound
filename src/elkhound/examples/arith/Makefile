# Makefile for arith example


# -------- external tools and libraries --------
# paths to the relevant subsystems
SMBASE        := ../../../smbase
AST           := ../../../ast
ELKHOUND      := ../..

# Elkhound runtime support library
LIBELKHOUND := $(ELKHOUND)/libelkhound.a


# -------- compiler and linker configuration --------
# preprocessing flags
CPPFLAGS := -I$(ELKHOUND) -I$(AST) -I$(SMBASE)

# C++ compilation flags; hijack from $(SMBASE) to find out
# whether I should supply -Wno-deprecatead
#CCFLAGS := -g -Wall -Wno-deprecated
CCFLAGS := $(shell $(SMBASE)/config.summary | grep CCFLAGS | sed 's/^.*: *//')

# linking flags
LDFLAGS := $(LIBELKHOUND) $(SMBASE)/libsmbase.a


# -------- targets --------
# main target
all: arith

# tell 'make' that it cannot compile arith.cc until after
# it has generated arith.gr.gen.h
arith.o: arith.gr.gen.h

# invoke the parser generator
arith.gr.gen.cc arith.gr.gen.h: arith.gr
	$(ELKHOUND)/elkhound -v -o arith.gr.gen arith.gr

# invoke the lexer generator
arithyy.cc: arith.lex
	flex -o$@ arith.lex

# compile a C++ source file
%.o: %.cc
	g++ -c $(CCFLAGS) $(CPPFLAGS) $*.cc

# link the driver code with the generated parser and lexer;
# also run it with a test input
arith: arith.o arithyy.o arith.gr.gen.o
	g++ -o $@ -g $^ $(LDFLAGS)
	echo "3 + 4 * 5" | ./arith
	echo "3 + 4 * 5" | ./arith printTree


clean:
	rm -f arith *.bin *.gen.* *.o

toolclean: clean
	rm -f arithyy.cc
