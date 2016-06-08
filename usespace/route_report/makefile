.KEEP_STATE:

all:    route_report

GCC     = gcc
CFLAGS = -Wall
LIB     =  -L. -lcurl -ljson -lxmlwrapper4c

object = md5.o common.o log_report.o ac_json.o report_core.o

route_report: $(object)
	$(GCC) $(CFLAGS) $(object) $(LIB) -o route_report

%.o:    %.c
	$(GCC) $(CFLAGS) -c $< -o $@

clean:
	${RM} *.o route_report
##gcc -Wall -o route_report report_core.c md5.c log_report.c common.c -L. -lcurl -ljson -lxmlwrapper4c