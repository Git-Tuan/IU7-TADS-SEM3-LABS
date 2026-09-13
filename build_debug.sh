#!/usr/bin/env bash

clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c parse_functions.c -o parse_functions.o -g
clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c calc_functions.c -o calc_functions.o -g
clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c main.c -o main.o -g

clang ./*.o -o ./app.exe
