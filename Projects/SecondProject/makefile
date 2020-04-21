CC = gcc
CFLAGS = -Wall
OBJS = U1.o 
XHDRS = U1.h
EXEC = U1

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c %.h $(XHDRS)
	$(CC) $(CFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
