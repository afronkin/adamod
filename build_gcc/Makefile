SRC_DIR=../src
INCS=$(SRC_DIR)/adamod.h $(SRC_DIR)/messages.h $(SRC_DIR)/modify.h
OBJS=adamod.o messages.o modify.o
PROGRAM=adamod

ADALNK_DIR=$(dir $(ADALNKX))/..

CC=gcc
CFLAGS=-O2 -Wall -Wextra -ansi -pedantic -Wshadow -Wpointer-arith -Wmissing-prototypes \
  -Wstrict-prototypes -Wold-style-definition -Wwrite-strings -Wno-long-long \
  -I$(SRC_DIR) -I$(SRC_DIR)/expat -I$(ADALNK_DIR)/inc
LDFLAGS=-L$(ADALNK_DIR)/lib -Wl,-rpath=$(ADALNK_DIR)/lib
LIBS=-lm -ladalnkx

.PHONY: all clean verify
.SUFFIXES: .c .o

all: $(PROGRAM)
	@echo $@ done.

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) $(LIBS) -o $@
	@echo $@ done.

clean:
	rm -f $(PROGRAM) $(OBJS) *.log
	@echo $@ done.

verify:
	./$(PROGRAM) -vv -t 88,100 -i 363846 "AE,4,A.test"
#	./$(PROGRAM) -vv -t 88,100 "EM,1,A,EN,1,A,EO,1,A.   "
#	./$(PROGRAM) -vvv -t 88,100 -l adamod.log -s "AW,6,A,D,FG,8,A,S,FG,8,A.READER2008010120081231" "FL,3,A,FK,3,A.aaabbb"

adamod.o: $(SRC_DIR)/adamod.c $(INCS)
	$(CC) $(CFLAGS) -c $< -o $@

messages.o: $(SRC_DIR)/messages.c $(INCS)
	$(CC) $(CFLAGS) -c $< -o $@

modify.o: $(SRC_DIR)/modify.c $(INCS)
	$(CC) $(CFLAGS) -c $< -o $@
