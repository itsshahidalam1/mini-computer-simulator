run.exe: main.o compiler.o memory.o processor.o
	gcc -o run.exe main.o compiler.o memory.o processor.o

main.o: main.c
	gcc -c main.c

compiler.o: compiler.c
	gcc -c compiler.c

memory.o: memory.c
	gcc -c memory.c

processor.o: processor.c
	gcc -c processor.c

clean:
	del *.o run.exe