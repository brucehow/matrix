# Makefile for the CITS3402 project
#
# The program can be built using the `make` bash command
# Please view README.md for more info on its usage
#
# @author Bruce How (22242664)
# @date 03/09/2019

PROGNAME = matrix
GCC = gcc-9 -std=c99 -Werror -Wall -pedantic -fopenmp
DEPENDENCIES = matrix.c

all: $(DEPENDENCIES)
	@$(GCC) -o $(PROGNAME) $(DEPENDENCIES)
	@echo "make: 'matrix' successfully built."

clean: $(DEPENDENCIES)
	@rm $(PROGNAME)
	@echo "make: 'matrix' has been removed."