VPATH=include
CC_FLAGS=-Iinclude
GCC=$(CC) $(CC_FLAGS)

linux_chip8: .chip8.o .cpu.o .input.o .screen.o .constant.o .linux_port.o
	$(GCC) .chip8.o .cpu.o .input.o .screen.o .constant.o .linux_port.o -o linux_chip8

.chip8.o: chip8.c cpu.h input.h screen.h constant.h port.h
	$(GCC) -c chip8.c -o .chip8.o

.cpu.o: cpu.c cpu.h screen.h input.h constant.h
	$(GCC) -c cpu.c -o .cpu.o

.input.o: input.c input.h constant.h
	$(GCC) -c input.c -o .input.o

.screen.o: screen.c screen.h constant.h port.h
	$(GCC) -c screen.c -o .screen.o

.constant.o: constant.c constant.h
	$(GCC) -c constant.c -o .constant.o

.linux_port.o: linux_port.c port.h
	$(GCC) -c linux_port.c -o .linux_port.o

test: clean chip8
	make clean

.PHONY: clean
clean:
	$(RM) .*.o chip8
