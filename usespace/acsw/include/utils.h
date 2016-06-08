#ifndef __UTILS_H__
#define __UTILS_H__

#define MAX_SEGMENT_LENGTH      1024


#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]
#define NIPQUAD_FMT "%u.%u.%u.%u"

#define MACQUAD(addr) \
	((unsigned char *)addr)[0], \
	((unsigned char *)addr)[1], \
	((unsigned char *)addr)[2], \
	((unsigned char *)addr)[3], \
	((unsigned char *)addr)[4], \
	((unsigned char *)addr)[5]
#define MACQUAD_FMT "%02x%02x%02x%02x%02x%02x"


typedef struct split_elem
{
    char val[MAX_SEGMENT_LENGTH];
} split_elem_t;

extern int split(char *input, char *delim, struct split_elem **elems, int *elems_num);

extern int mac_str_to_bin( char *str, unsigned char *mac);

extern char * str_reverse_char(char *input, char delim);

extern int do_get_command(const char *cmd, char *buffer, int lenght);

#endif

