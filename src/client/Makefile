# CC = gcc
CFLAGS = -g
SRCS = display_process.c register_member.c raw_mode.c choice_menu.c client_sock.c display_makeroom.c display_joinroom.c display_chatroom.c main.c
OBJS = $(SRCS:.c=.o)
INCLUDE = -I../../include
BUILD_OBJS = ../../build/client/display_process.o ../../build/client/register_member.o ../../build/client/raw_mode.o \
			 ../../build/client/choice_menu.o ../../build/client/client_sock.o ../../build/client/display_makeroom.o \
			 ../../build/client/display_chatroom.o ../../build/client/display_joinroom.o\
			 ../../build/data/data_message.o ../../build/data/data_room.o ../../build/data/data_user.o \
			 ../../build/client/main.o \
			 ../../build/parser/parse_command.o ../../build/parser/tostring_command.o
			 
.PHONY: obj clean

output: $(BUILD_OBJS)
	$(CC) -o $@ $(BUILD_OBJS)

obj : $(OBJS)
	mkdir -p ../../build/client/
	mv $(OBJS) main.o ../../build/client/

.c.o:
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

clean:
	cd ../../build/client && rm -f $(OBJS)