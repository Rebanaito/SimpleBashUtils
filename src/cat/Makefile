CC = gcc
CF = -std=c11 -Wall -Werror -Wextra

s21_cat: s21_cat.c s21_cat.h ../common/s21_common.c
	$(CC) $(CF) s21_cat.c -o s21_cat

test: s21_cat
	bash test_func_cat.sh 2>/dev/null
	cppcheck --enable=all --suppress=missingIncludeSystem s21_cat.c
	clang-format -style=google -n s21_cat.c	

test_leak: test_leak_cat.sh s21_cat
	bash test_leak_cat.sh 2>/dev/null

clean:
	rm -rf s21_cat

rebuild:
	make clean
	make
