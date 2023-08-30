CC = gcc
CF = -std=c11 -Wall -Werror -Wextra

s21_grep: s21_grep.c s21_grep.h
	$(CC) $(CF) s21_grep.c -o s21_grep

test: s21_grep
	bash test_func_grep.sh 2>/dev/null
	cppcheck --enable=all --suppress=missingIncludeSystem s21_grep.c
	clang-format -style=google -n s21_grep.c

test_leak: s21_grep
	bash test_leak_grep.sh 2>/dev/null

clean:
	rm -rf s21_grep

rebuild:
	make clean
	make s21_grep
