chip8.exe: chip8.o cpu.o input.o screen.o
	gcc chip8.o cpu.o input.o screen.o -o chip8.exe

chip8.o: chip8.c cpu.h input.h screen.h
	gcc -c chip8.c -o chip8.o

cpu.o: cpu.c cpu.h screen.h input.h
	gcc -c cpu.c -o cpu.o

input.o: input.c input.h
	gcc -c input.c -o input.o

screen.o: screen.c screen.h
	gcc -c screen.c -o screen.o

