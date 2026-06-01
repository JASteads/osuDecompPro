@echo off

gcc ../Code/main.c -o ../main.o -c
gcc ../Code/osuDecompPro.c -o ../osuDecompPro.o -c
gcc -o ../osuDecompPro ../main.o ../osuDecompPro.o