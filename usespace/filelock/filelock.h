#ifndef _IP_DETECT_H_
#define _IP_DETECT_H_

#define DEV_ERR                     -1 
#define DEV_OK						0

struct filelock_info
{
    char    *lockfile;
    char    *dfile;
	int     get;
	int     set;
	int     help;
};

#endif

