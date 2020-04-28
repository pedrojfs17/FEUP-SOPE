CC = gcc
CFLAGS = -pthread
OBJS = U1.o ../utils/logs.o ../utils/utils.o
XHDRS = U1.h defs.h ../utils/logs.h ../utils/utils.h
EXEC = U1

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c %.h $(XHDRS)
	$(CC) $(CFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)
