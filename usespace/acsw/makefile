.KEEP_STATE:

all:    acsw

GCC     = g++
CFLAGS = -Wall
LIB     =  -L. -lcurl -ljson -lopt -lxmlwrapper4c

object = utils.o ac_json.o common.o ac_mac_auth.o ac_ip_classify.o ac_report.o ac_cache.o ac_core.o

acsw: $(object) md5.o
	$(GCC) $(CFLAGS) $(object) md5.o $(LIB) -o acsw

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@
	
md5.o:md5.cpp
	g++ -c -o md5.o md5.cpp

clean:
	${RM} *.o acsw
	
