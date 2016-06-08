.KEEP_STATE:


all:    filelock

GCC     = gcc
CFLAGS = -Wall -DDEBUG -I/root/src/opt-3.19/src
LIB     = -L. -l opt

object = filelock.o

filelock: $(object)
	$(GCC) $(CFLAGS) $(object) $(LIB) -o filelock

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	${RM} *.o filelock

