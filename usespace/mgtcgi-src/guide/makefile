.KEEP_STATE:

all:    guide.cgi guide-dump

GCC     = gcc
CFLAGS = -Wall -DDEBUG
LIB = -L./ -lcgic -lxmlwrapper4c

object = debug.o utils.o guide_main.o guide_welcome.o guide_lan.o guide_server.o guide_wan.o \
		guide_primary.o guide_slave.o guide_appbind.o guide_tc.o guide_zone.o guide_dnat.o \
		guide_submit.o

object1= debug.o utils.o guide_dump.o

guide.cgi: $(object)
	$(GCC) $(CFLAGS) $(object) $(LIB) -o guide.cgi

guide-dump:$(object1)
	$(GCC) $(CFLAGS) $(object1) -L./ -lxmlwrapper4c -o guide-dump

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
clean:
	${RM} *.o guide.cgi guide-dump
	
