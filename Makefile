CC = gcc
CPPFLAGS = -MMD
CFLAGS = -Wall -Wextra -g `pkg-config --cflags bobgl`
LDFLAGS = -lm
LDLIBS = `pkg-config --libs bobgl`


SRC = main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}
EXE = ${SRC:.c=}

all: main

main: main.o
main.o: main.c

-include ${DEP}

.PHONY: clean

clean:
	${RM} ${OBJ}
	${RM} ${DEP}

reset: clean
	${RM} ${EXE}

# END
