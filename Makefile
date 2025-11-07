all: sound.exe

sound.exe: main.c generator.c
	gcc -o sound.exe main.c generator.c -lwinmm

clean:
	rm -rf /F /Q *.exe *.o

run: all
	sound.exe