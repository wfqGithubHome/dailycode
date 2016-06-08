.KEEP_STATE:


all:    authctl

GCC	= gcc
CFLAGS = -Wall -DDEBUG
LIB     = -L /usr/local/lib -l opt 

object = authctl.o 

authctl: $(object)
	$(GCC) $(CFLAGS) authctl.o $(LIB) -o authctl

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	${RM} *.o authctl

