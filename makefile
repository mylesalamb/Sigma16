OBJDIR = obj
SRCDIR = src
SRC = $(shell ls src/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRC))

CC = gcc
CCFLAGS = -O2
HEADERS = -Iinclude
EXEC = sigma16
LIB = -lncurses
$(EXEC): $(OBJ)
	$(CC) $(CCFLAGS) $(HEADERS) -o $(EXEC) $(OBJ) $(LIB)

$(OBJDIR)/%.o: %.c
	$(CC) -c $(CCFLAGS)  $(HEADERS)  -o $@ $< $(LIB)


.PHONY: clean again

again: clean $(EXEC)

clean:
	rm $(OBJDIR)/*.o $(EXEC)


vpath %.c src/
vpath %.o obj/

