#USO: Reemplazar solamente los valores de las primeras 3 lineas:
#	En EXEC escribir el nombre del TDA/TP esperado
#	En CFILES escribir los archivos.c con los que se trabaja
#	En HFILES escribir los archivos.h con los que se trabaja

#Tiene 2 funcionalidades:
#	make run: Corre el programa
#	make valgrind: Chequea perdida de memoria

EXEC = analog
CFILES = analog.c strutil.c heap.c hash.c abb.c pila.c lista.c cola.c
HFILES = strutil.h heap.h hash.h abb.h pila.h lista.h cola.h

CC = gcc
CFLAGS = -g -std=c99 -Wall
CFLAGS += -Wconversion -Wno-sign-conversion -pedantic -Werror -Wconversion -Wbad-function-cast -Wshadow
CFLAGS += -Wpointer-arith -Wunreachable-code -Wformat=2 
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes
GDBFLAGS = -tui
LOG_VALGRIND = 2>&1 | tee valgrind.txt
LOG_GCC = 2>&1 | tee gcc.txt
EJECUCION = 0<ejecucion.txt
all: $(EXEC)

run: all
	./$(EXEC) $(LOG_GCC)

runtime: all
	time ./$(EXEC)

valgrind: all
	$(EXEC) $(LOG_GCC)
	valgrind $(VFLAGS) ./$(EXEC) $(EJECUCION) $(LOG_VALGRIND) 

debug: all
	gdb $(GDBFLAGS) ./$(EXEC) 

todo: all
	valgrind $(VFLAGS) ./$(EXEC) $(LOG_VALGRIND)

clean:
	rm $(EXEC) *.txt

$(EXEC): $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) $(CFILES) -o $(EXEC)
