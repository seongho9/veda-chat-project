CC = gcc
CFLAGS = -g
INCLUDE = -I../../include
SRCS = test_parse_command.c
OBJS = $(SRCS:.c=.o)
TESTS = $(SRCS:.c=.out)
LINKS = ../../build/parser/parse_command.o ../../build/parser/tostring_command.o
INCLUDE = -I../../include

.PHONY: test clean

$(TESTS): $(OBJS)
	$(CC) -o $@ $(LINKS) $<
.c.o:	
	$(CC) -c $(CFLAGS) $(INCLUDE) $<

clean:
	rm -f $(OBJS) $(TESTS)