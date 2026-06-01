@echo off

gcc -g ../Code/main.c -o ../main.o -c
gcc -g ../Code/osuDecompPro.c -o ../osuDecompPro.o -c
gcc -g -o ../osuDecompPro ../main.o ../osuDecompPro.o