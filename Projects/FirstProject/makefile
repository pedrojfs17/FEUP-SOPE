CC = gcc
CFLAGS = -Wall
OBJS = simpledu.o register.o utils.o
XHDRS = simpledu.h register.h utils.h
EXEC = simpledu

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c %.h $(XHDRS)
	$(CC) $(CFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
