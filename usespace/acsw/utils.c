#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "include/utils.h"

static void set_reuid()
{
	uid_t uid ,euid; 
	uid = getuid(); 
	euid = geteuid(); 
	setreuid(euid, uid);
}

int split(char *input, char *delim, struct split_elem **elems, int *elems_num)
{
    char *buf = NULL;
    int  buf_len = strlen(input);
    struct split_elem *elem = NULL;
    int delim_len = strlen(delim);
    char *cur, *res;
    int cnt = 1, i;
    int ret = -1;

    if (input == NULL || delim == NULL || elems == NULL)
    {
        goto exit;
    }

    buf = (char *)malloc(buf_len+10);
    if (buf == NULL)
    {
        goto exit;
    }

    strcpy(buf, input);
    cur = buf;
    while (*cur != '\0')
    {
        res = strstr(cur, delim);
        if (res == NULL)
        {
            break;
        }

        cnt++;
        cur = res + delim_len;
        *res = '\0';   
    }

    if (cnt <= 0)
    {
        goto exit;
    }

    *elems_num = cnt;
    *elems = (struct split_elem *)malloc(sizeof(struct split_elem) * cnt);
    if (*elems == NULL)
    {
        goto exit;
    }

    elem = *elems;
    cur = buf;
    for (i = 0; i < cnt; i++)
    {
        strncpy(elem[i].val, cur, MAX_SEGMENT_LENGTH);
        cur += (strlen(cur) + delim_len); 
    }

    ret = 0;
exit:
    if (buf)
    {
        free(buf);
    }
    
    return ret;
}

#if 0
int mac_str_to_bin( char *str, unsigned char *mac)
{
    int i;
    char *s, *e;

    if ((mac == NULL) || (str == NULL))
    {
        return -1;
    }

    s = (char *) str;
    for (i = 0; i < 6; ++i)
    {
        mac[i] = s ? strtoul (s, &e, 16) : 0;
        if (s)
           s = (*e) ? e + 1 : e;
    }
    return 0;
}
#endif

int mac_str_to_bin( char *str, unsigned char *mac)
{
    int i;
    char *s, *e;

    int idx = 0;
    size_t len = strlen(str);
    char buf[32] = {0};
    
    if ((mac == NULL) || (str == NULL))
    {
        return -1;
    }
    
    if (len != 12)
    {
        return -1;
    }

    for (i = 0; i < (int)len; i++)
    {
        if (i>0 && i%2==0)
        {
            buf[idx] = '-';
            idx++;
        }

        buf[idx] = str[i];
        idx++;
    }

    s = (char *) buf;
    for (i = 0; i < 6; ++i)
    {
        mac[i] = s ? strtoul (s, &e, 16) : 0;
        if (s)
           s = (*e) ? e + 1 : e;
    }
    return 0;
}

char * str_reverse_char(char *input, char delim)
{
    int len = strlen(input);
    int i;
    char *result = NULL;

    for (i=len-1; i>=0; i--)
    {
        if (input[i] != delim)
        {
            continue;
        }

        if (i>0 && input[i-1]!=delim)
        {
            result = &input[i];
        }

        break;
    }
    
    return result;
}

int do_get_command(const char *cmd, char *buffer, int lenght)
{
	FILE *pp = NULL;
	
	set_reuid();
	pp=popen(cmd,"r");
	if (pp != NULL){
		fgets(buffer, lenght, pp);
		buffer[strlen(buffer)-1] = 0;
	}
	pclose(pp);
	return 0;
}

