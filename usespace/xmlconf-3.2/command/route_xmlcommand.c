#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <error.h>

#include "../libxmlwrapper4c/route_xmlwrapper4c.h"

/*
	����:
	 gcc -o xmlconf  -L.  -l xmlwrapper4c  xmlcommand.c
*/

int main(int argc,char** argv)
{
	return  route_xml_do_command(argc, argv);
}


