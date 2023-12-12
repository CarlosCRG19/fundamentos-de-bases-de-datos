CC = gcc -g
CFLAGS = -Wall -Wextra -pedantic

# recompile if this header changes
HEADERS = library.h book_index.h database.h commands.h loop.h

EXE = library
OBJ = $(EXE).o book_index.o database.o commands.o loop.o

compile : $(EXE)

# compile all files ending in *.c
%.o: %.c $(HEADERS)
	@echo Compiling $<...
	$(CC) $(CFLAGS) -c -o $@ $<

# link binary
$(EXE): $(DEPS) $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(LDLIBS)

clean:
	rm -f *.o core $(EXE)

run:
	./$(EXE)
