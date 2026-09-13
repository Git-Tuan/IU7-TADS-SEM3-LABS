#!/usr/bin/env bash
KEYS="-c -Wall -Werror -Wextra -Wpedantic -g -fno-omit-frame-pointer -fsanitize=undefined"
clang $KEYS main.c -o main.o
clang $KEYS parse_functions.c -o parse_functions.o
clang $KEYS calc_functions.c -o calc_functions.o

clang ./*.o -o ./app.exe -fsanitize=undefined