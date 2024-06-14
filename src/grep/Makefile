CC=gcc
CFLAGS=-Wall -Werror -Wextra -std=c11

all: s21_grep clean_obj

s21_grep: s21_grep.o
	$(CC) $(FLAGS) -o s21_grep s21_grep.o

clean: clean_obj
	rm -rf s21_grep .clang-format

clean_obj:
	rm -rf *.o

clang-i:
	cp ../../materials/build/scripts/.clang-format .clang-format
	clang-format -i s21_grep.c s21_grep.h

clang-n:
	cp ../../materials/build/scripts/.clang-format .clang-format
	clang-format -n s21_grep.c s21_grep.h

test:
	bash tests.sh

val:
	bash test_leaks.sh

cpp:
	cppcheck —enable=all —suppress=missingIncludeSystem s21_grep.c