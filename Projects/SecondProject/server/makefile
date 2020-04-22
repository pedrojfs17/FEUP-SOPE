CC = gcc
CFLAGS = -pthread
OBJS = Q1.o
XHDRS = Q1.h defs.h
EXEC = Q1

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c %.h $(XHDRS)
	$(CC) $(CFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)