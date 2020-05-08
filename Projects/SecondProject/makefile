CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -Wno-unused-parameter -pthread -lpthread
UOBJ = client/U2.o
QOBJ = server/Q2.o
COBJ = utils/utils.o utils/logs.o
TARGETS = U2 Q2

all: U2 Q2

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

U2: $(UOBJ) $(COBJ)
	$(CC) $(CFLAGS) $(COBJ) $(UOBJ) -o $@

Q2: $(QOBJ) $(COBJ)
	$(CC) $(CFLAGS) $(COBJ) $(QOBJ) -o $@

.PHONY : clean
clean:
	rm $(UOBJ) $(QOBJ) $(COBJ) $(TARGETS)