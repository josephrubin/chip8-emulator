chip8: .chip8.o .cpu.o .input.o .screen.o .constant.o .port.o
	gcc .chip8.o .cpu.o .input.o .screen.o .constant.o .port.o -o chip8

.chip8.o: chip8.c cpu.h input.h screen.h constant.h
	gcc -c chip8.c -o .chip8.o

.cpu.o: cpu.c cpu.h screen.h input.h constant.h
	gcc -c cpu.c -o .cpu.o

.input.o: input.c input.h constant.h
	gcc -c input.c -o .input.o

.screen.o: screen.c screen.h constant.h
	gcc -c screen.c -o .screen.o

.constant.o: constant.c constant.h
	gcc -c constant.c -o .constant.o

.port.o: port.c port.h
	gcc -c port.c -o .port.o

clean:
	rm -f *.o chip8
