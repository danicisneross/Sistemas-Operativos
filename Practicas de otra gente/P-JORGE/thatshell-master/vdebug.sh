#!/bin/bash
# Compiles main program
gcc -g -Wall shell.c arrayimpl.c create.c listmem.c deleteimpl.c listimpl.c memory.c proccess.c plistimpl.c -o shell.out

# Compiles secondary programs, from P1
gcc -g -Wall listimpl.c list.c -o list.out
gcc -g -Wall deleteimpl.c delete.c -o delete.out

# Runs main program with Valgrind
valgrind --leak-check=full --leak-resolution=med --track-origins=yes --vgdb=no ./shell.out

