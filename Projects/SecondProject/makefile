CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -Wno-unused-parameter -pthread -lpthread
UOBJ = client/U1.o
QOBJ = server/Q1.o
COBJ = utils/utils.o utils/logs.o
TARGETS = U1 Q1

all: U1 Q1

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

U1: $(UOBJ) $(COBJ)
	$(CC) $(CFLAGS) $(COBJ) $(UOBJ) -o $@

Q1: $(QOBJ) $(COBJ)
	$(CC) $(CFLAGS) $(COBJ) $(QOBJ) -o $@

.PHONY : clean
clean:
	rm $(UOBJ) $(QOBJ) $(COBJ) $(TARGETS)