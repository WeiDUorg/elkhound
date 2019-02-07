# Makefile

ELKHOUND := ../..

all: tokens.tok occ2.ml

# generate token codes for Elkhound
tokens.tok: tokens.h
	$(ELKHOUND)/make-tok Token <tokens.h >tokens.tok

# run elkhound on a grammar spec
%.ml: %.gr tokens.tok
	$(ELKHOUND)/elkhound -ocaml -v $*.gr
	rm -f $*.mli

clean:
	rm -f tokens.tok occ2.ml
