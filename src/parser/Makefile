# CC = gcc
CFLAGS = -g
INCLUDE = -I../../include
SRCS = parse_command.c tostring_command.c
OBJS = $(SRCS:.c=.o)
INCLUDE = -I../../include

.PHONY: obj clean

obj : $(OBJS)
	mkdir -p ../../build/parser/
	mv $(OBJS) ../../build/parser/
.c.o:	
	$(CC) -c $(CFLAGS) $(INCLUDE) $<
clean:
	cd ../../build/parser && rm -f $(OBJS)