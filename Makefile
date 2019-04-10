
LDFLAGS:=`pkg-config gtk+-3.0 --libs` -rdynamic -lexpat

CFLAGS:=`pkg-config gtk+-3.0 --cflags`
CFLAGS+= -Wfatal-errors -pedantic -Wall -Wextra -Werror
CFLAGS+= -std=c99 -Iinclude -g
# because glib-compile-resources don't play nice...
CFLAGS+=  -Wno-overlength-strings

SRC:=$(wildcard src/*.c)
OBJ:=$(SRC:src/%.c=obj/%.o)
INC:=$(wildcard include/*.h)

# GCR should be on the path...
GCR=glib-compile-resources
CC=gcc

cLogicToy: $(OBJ)
	$(CC) $(OBJ) -o cLogicToy $(LDFLAGS)

$(OBJ): obj/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# this one file needs a special case as it has many specific dependencies
src/resources.c: res/resource.xml res/mainUI.glade img/And.png img/In.png img/Or.png img/Xor.png img/inwire.png img/on.png img/Cross.png img/Not.png img/Out.png img/bg.png img/lon.png img/outwire.png
	$(GCR) res/resource.xml --generate-source --target=src/resources.c --generate

.PHONY:	clean 
clean:
	rm obj/* -f
	rm cLogicToy -f

style: $(SRC) $(INC)
	astyle -A10 -s4 -S -p -xg -j -z2 -n src/* include/*
