.KEEP_STATE:


all:    xmlconf route-xmlconf guide-xmlconf

#CXX     = /usr/local/arm-xscale/bin/armv5b-softfloat-linux-g++ -mbig-endian
#GCC	= /usr/local/arm-xscale/bin/armv5b-softfloat-linux-gcc -mbig-endian
#CXXFLAGS = -Wall -Wno-deprecated -I ../opt-3.19/src/
#LIB     = -L ../opt-3.19/src/ -l opt 
CXX     = g++
GCC	= gcc
CXXFLAGS = -g -Wall -Wno-deprecated 
CXXLIBFLAGS = -fPIC -shared -pthread
LIB     = -L /usr/local/lib/ -l xmlwrapper4c 

xmlconf: xmlcommand.c
	$(GCC) -o xmlconf  $(LIB)  xmlcommand.c	
route-xmlconf: route_xmlcommand.c
	$(GCC) -o route-xmlconf  $(LIB)  route_xmlcommand.c	

guide-xmlconf:guide_xmlcommand.c
	$(GCC) -o guide-xmlconf  $(LIB)  guide_xmlcommand.c	
%.o:    %.cc
	$(CXX) $(CXXFLAGS)  -c $< -o $@

clean:
	${RM} *.o  xmlconf
	${RM} *.o  route-xmlconf
	${RM} *.o  guide-xmlconf
