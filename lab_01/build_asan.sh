#!/usr/bin/env bash
KEYS="-c -Wall -Werror -Wextra -Wpedantic -g -fno-omit-frame-pointer -fsanitize=address"

clang $KEYS main.c -o main.o
clang $KEYS parse_functions.c -o parse_functions.o
clang $KEYS errors.c -o errors.o
clang $KEYS numbers_io.c -o numbers_io.o
clang $KEYS calc_functions.c -o calc_functions.o

clang ./*.o -o ./app.exe -fsanitize=address