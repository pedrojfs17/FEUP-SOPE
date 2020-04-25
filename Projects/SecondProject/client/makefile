CC = gcc
CFLAGS = -pthread
OBJS = U1.o ../logs/logs.o
XHDRS = U1.h defs.h ../logs/logs.h
EXEC = U1

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c %.h $(XHDRS)
	$(CC) $(CFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
