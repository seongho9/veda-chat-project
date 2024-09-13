CC = gcc
CFLAGS = -g
SRCS = accept_proc.c client_proc.c 
OBJS = $(SRCS:.c=.o)
INCLUDE = -I../../include
BUILD_OBJS = ../../build/server/accept_proc.o ../../build/server/client_proc.o ../../build/data/data_message.o ../../build/data/data_room.o ../../build/data/data_user.o main.o
.PHONY: obj clean

output: $(BUILD_OBJS)
	gcc -o $@ $(BUILD_OBJS)

main.o: main.c
	gcc -c -g $(INCLUDE) $<
obj : $(OBJS)
	mkdir -p ../../build/server/
	mv $(OBJS) ../../build/server/

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

clean:
	cd ../../build/server && rm -f $(OBJS)