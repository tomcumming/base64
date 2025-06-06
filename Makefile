.PHONY: format-code

test: main.c base64.c
	clang --std=c11 -Wall main.c base64.c -o test
	./test

format:
	clang-format -i *.c
