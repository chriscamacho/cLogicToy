
LDFLAGS:=`pkg-config gtk+-3.0 --libs` -rdynamic -lexpat

CFLAGS:=`pkg-config gtk+-3.0 --cflags`
CFLAGS+= -Wfatal-errors -pedantic -Wall -Wextra -Werror
CFLAGS+= -std=c99 -Iinclude -g

SRC:=$(wildcard src/*.c)
OBJ:=$(SRC:src/%.c=obj/%.o)
INC:=$(wildcard include/*.h)

CC=gcc

cLogicToy: $(OBJ)
	$(CC) $(OBJ) -o cLogicToy $(LDFLAGS)

$(OBJ): obj/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY:	clean 
clean:
	rm obj/* -f
	rm cLogicToy -f

style: $(SRC) $(INC)
	astyle -A10 -s4 -S -p -xg -j -z2 -n src/* include/*
