.KEEP_STATE:


all:    mac-bind

GCC     = gcc
CFLAGS = -Wall -DDEBUG -I/root/src/opt-3.19/src
LIB     = -l opt -L. -lxmlwrapper4c

object = mac-bind.o

mac-bind: $(object)
	$(GCC) $(CFLAGS) mac-bind.o $(LIB) -o mac-bind

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	${RM} *.o mac-bind

