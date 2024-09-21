# CC = gcc
CFLAGS = -g
INCLUDE = -I../../include
SRCS = data_message.c data_room.c data_user.c
OBJS = $(SRCS:.c=.o)

.PHONY: obj clean

obj : $(OBJS)
	mkdir -p ../../build/data/
	mv $(OBJS) ../../build/data/

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

clean :
	cd ../../build/data && rm -f $(OBJS)