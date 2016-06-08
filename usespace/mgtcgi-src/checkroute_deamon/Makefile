all:	checkroute
GCC	= gcc
FLAGS = -g -Wall 

checkroute: checkroute_deamon.c 
	$(GCC) $(FLAGS)  -o checkroute checkroute_deamon.c init.c common.c -L../../xmlconf-3.2/ -lxmlwrapper4c
clean:
	rm -fr  *.o  checkroute
