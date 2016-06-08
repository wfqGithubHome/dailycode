.KEEP_STATE:


all:    lcmshow

GCC     = gcc
CFLAGS = -Wall -DDEBUG -I/root/src/opt-3.19/src
LIB     = -l opt

object = lcmshow.o

lcmshow: $(object)
	$(GCC) $(CFLAGS) $(object) $(LIB) -o lcmshow

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	${RM} *.o lcmshow

