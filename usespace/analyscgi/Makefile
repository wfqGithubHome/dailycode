all:    flowstat

CFLAGS=-g -Wall
GCC=gcc
LIBS=-L./ -lcgic -lxmlwrapper4c

object = common.o channelstat.o flowstat.o 

flowstat: $(object)
	$(GCC) $(CFLAGS) $(object)$(LIBS) -o flowstat.cgi

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	${RM} *.o flowstat.cgi

