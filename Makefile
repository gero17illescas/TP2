#USO: Reemplazar solamente los valores de las primeras 3 lineas:
#	En EXEC escribir el nombre del TDA/TP esperado
#	En CFILES escribir los archivos.c con los que se trabaja
#	En HFILES escribir los archivos.h con los que se trabaja

#Tiene 2 funcionalidades:
#	make run: Corre el programa
#	make valgrind: Chequea perdida de memoria

EXEC = heap
CFILES = main.c heap.c pruebas_heap.c testing.c
HFILES = heap.h testing.h

CC = gcc
CFLAGS = -g -std=c99 -Wall
CFLAGS += -Wconversion -Wno-sign-conversion -Werror -Wbad-function-cast -Wshadow
CFLAGS += -Wpointer-arith -Wunreachable-code -Wformat=2 
VFLAGS = --leak-check=full --track-origins=yes --show-reachable=yes
GDBFLAGS = -tui
LOG_VALGRIND = 2>&1 | tee valgrind.txt
LOG_GCC = 2>&1 | tee gcc.txt

all: $(EXEC)

run: all
	./$(EXEC) $(LOG_GCC)

runtime: all
	time ./$(EXEC)

valgrind: all
	$(EXEC) $(LOG_GCC)
	valgrind $(VFLAGS) ./$(EXEC) $(LOG_VALGRIND)

debug: all
	gdb $(GDBFLAGS) ./$(EXEC) 

todo: all
	valgrind $(VFLAGS) ./$(EXEC) $(LOG_VALGRIND)
	
$(EXEC): $(CFILES) $(HFILES)
	$(CC) $(CFLAGS) $(CFILES) -o $(EXEC)
