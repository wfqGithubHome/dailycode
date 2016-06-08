CFLAGS=-g -Wall
CC=gcc
LIBS=-L../lib/ -lcgic -lxmlwrapper4c

all: management.cgi
management.cgi: mgtcgi_management.o mgtcgi_filed_check.o
	gcc mgtcgi_management.o mgtcgi_filed_check.o -L./ -lcgic -lxmlwrapper4c

clean:
	rm -f *.o *.cgi a.out

