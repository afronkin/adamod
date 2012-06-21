SRC_DIR=../src
OBJS=adamod.o
PROGRAM=adamod

CC=gcc
CFLAGS=-O2 -Wall -Wextra -ansi -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes \
  -Wstrict-prototypes -Wold-style-definition -Wwrite-strings -Wno-long-long \
  -I$(SRC_DIR) -I$(SRC_DIR)/expat -I/sag/ada/v51404/inc
LDFLAGS=-L/sag/ada/v51404/lib -Wl,-rpath=/sag/ada/v51404/lib
LIBS=-lm -ladalnkx

.PHONY: all clean verify
.SUFFIXES: .c .o

all: $(PROGRAM)
	@echo $@ done.

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	@echo $@ done.

clean:
	rm -f $(PROGRAM) $(OBJS)
	@echo $@ done.

verify:
	./$(PROGRAM) -vv -t 88,12 444136 "GM6,23." "013Администрирование БД"

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@
