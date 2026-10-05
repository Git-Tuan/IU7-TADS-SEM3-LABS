#!/usr/bin/env bash

clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c parse_functions.c -o parse_functions.o -O2
clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c calc_functions.c -o calc_functions.o -O2
clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c main.c -o main.o -O2
clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c numbers_io.c -o numbers_io.o -O2
clang -std=c99 -Wall -Werror -Wextra -Wpedantic -c errors.c -o errors.o -O2

clang ./*.o -o ./app.exe
