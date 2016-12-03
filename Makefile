INC=-Iinclude
CFLAGS=$(INC)

PARSERDIR=input_parse

SRCS=utils.c dsh.c builtins.c input.c output.c
OBJS:=$(SRCS:.c=.o)

all: clean dsh

dsh: $(OBJS) siparse.a
	cc $(CFLAGS) $(OBJS) siparse.a -o $@ 

%.o: %.c
	cc $(CFLAGS) -c $<

siparse.a:
	$(MAKE) -C $(PARSERDIR)

clean:
	rm -f dsh *.o siparse.a
