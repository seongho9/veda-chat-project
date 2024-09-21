#CC = aarch64-linux-gnu-gcc-12
CFLAGS = -g
SRCS = accept_proc.c client_proc.c server_signal_handler.c session.c main.c
OBJS = $(SRCS:.c=.o)
INCLUDE = -I../../include
BUILD_OBJS = ../../build/server/accept_proc.o ../../build/server/client_proc.o ../../build/server/server_signal_handler.o ../../build/server/session.o \
			 ../../build/data/data_message.o ../../build/data/data_room.o ../../build/data/data_user.o ../../build/server/main.o \
			 ../../build/parser/parse_command.o ../../build/parser/tostring_command.o
.PHONY: obj clean

output: $(BUILD_OBJS)
	$(CC) -o $@ $(BUILD_OBJS)

# main.o: main.c
# 	$(CC) -c $(CFLAGS) $(INCLUDE) $<
obj : $(OBJS)
	mkdir -p ../../build/server/
	mv $(OBJS) ../../build/server/

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

clean:
	cd ../../build/server && rm -f $(OBJS)