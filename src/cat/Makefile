SHELL := /bin/bash
FILE=test.txt
CC=gcc
CFLAGS=-Wall -Wextra -Werror -pedantic -Ofast -std=c11 -IIncludes

all: s21_cat

s21_cat: s21_cat.o CatFlags.o
	$(CC) $(CFLAGS) s21_cat.o CatFlags.o -o s21_cat -lm

CatFlags.o: CatFlags.c CatFlags.h
	$(CC) $(CFLAGS) -c CatFlags.c -o CatFlags.o

s21.cat.o: s21_cat.c
	$(CC) $(CFLAGS) -c s21_cat.c -o s21_cat.o

tests: 
	-diff <(./s21_cat $(FILE)) <(cat $(FILE))
	-diff <(./s21_cat -b $(FILE)) <(cat -b $(FILE)) 
	-diff <(./s21_cat --number-nonblank $(FILE)) <(cat -b $(FILE))
	-diff <(./s21_cat -e $(FILE)) <(cat -e $(FILE))
	-diff <(./s21_cat -E $(FILE)) <(cat -E $(FILE))  
	-diff <(./s21_cat -n $(FILE)) <(cat -n $(FILE)) 
	-diff <(./s21_cat --number $(FILE)) <(cat -n $(FILE))
	-diff <(./s21_cat -s $(FILE)) <(cat -s $(FILE)) 
	-diff <(./s21_cat --squeeze-blank $(FILE)) <(cat -s $(FILE))
	-diff <(./s21_cat -t $(FILE)) <(cat -t $(FILE))
	-diff <(./s21_cat -T $(FILE)) <(cat -T $(FILE)) 
	-diff <(./s21_cat -v $(FILE)) <(cat -v $(FILE)) 

clean:
	rm -f *.o s21_cat

rebuild: clean all
