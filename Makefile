VPATH=include
CC_FLAGS=-Iinclude
GCC=gcc $(CC_FLAGS)

chip8: .chip8.o .cpu.o .input.o .screen.o .constant.o .port.o
	$(GCC) .chip8.o .cpu.o .input.o .screen.o .constant.o .port.o -o chip8

.chip8.o: chip8.c cpu.h input.h screen.h constant.h
	$(GCC) -c chip8.c -o .chip8.o

.cpu.o: cpu.c cpu.h screen.h input.h constant.h
	$(GCC) -c cpu.c -o .cpu.o

.input.o: input.c input.h constant.h
	$(GCC) -c input.c -o .input.o

.screen.o: screen.c screen.h constant.h
	$(GCC) -c screen.c -o .screen.o

.constant.o: constant.c constant.h
	$(GCC) -c constant.c -o .constant.o

.port.o: port.c port.h
	$(GCC) -c port.c -o .port.o

test: clean chip8
	make clean

.PHONY: clean
clean:
	$(RM) .*.o chip8
