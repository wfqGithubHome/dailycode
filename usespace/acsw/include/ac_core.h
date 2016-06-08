#ifndef __AC_CORE_H__
#define __AC_CORE_H__

#define AC_REPORT_LONG_NAME        		"AC-REPORT"
#define AC_REPORT_NAME                  "AC-REPORT"
#define AC_REPORT_DESCRIPTION      		"AC-REPORT"
#define AC_REPORT_VERSION             	"1.0.0"


#define AC_COMMAND_LENGTH     256

struct ac_report_config_info
{
    char    *url;
    
    int     setting;
    int     portal;
    int     state;
	int     warning;
	int     config;
    int     app;
    int     appinit;
    int     webfile;
    int     portmap;
    
	int     help;
};

#endif

