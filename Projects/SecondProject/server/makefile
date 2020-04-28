CC = gcc
CFLAGS = -pthread
OBJS = Q1.o ../utils/logs.o ../utils/utils.o
XHDRS = Q1.h defs.h ../utils/logs.h ../utils/utils.h
EXEC = Q1

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

%.o: %.c %.h $(XHDRS)
	$(CC) $(CFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS) $(EXEC)