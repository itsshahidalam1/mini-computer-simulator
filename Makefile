run: main.o os.o compiler.o memory.o processor.o
	gcc -o run main.o os.o compiler.o memory.o processor.o

main.o: main.c
	gcc -c main.c

os.o: os.c
	gcc -c os.c

compiler.o: compiler.c
	gcc -c compiler.c

memory.o: memory.c
	gcc -c memory.c

processor.o: processor.c
	gcc -c processor.c

clean:
	rm -f *.o run