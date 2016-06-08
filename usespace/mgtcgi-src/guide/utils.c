#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "include/utils.h"

int guide_split(char *input, char *delim, struct guide_split_elem **elems, int *elems_num)
{
    char *buf = NULL;
    int  buf_len = strlen(input);
    struct guide_split_elem *elem = NULL;
    int delim_len = strlen(delim);
    char *cur, *res;
    int cnt = 1, i;
    int ret = -1;

    if (input == NULL || delim == NULL || elems == NULL)
    {
        goto exit;
    }

    buf = malloc(buf_len+10);
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
    *elems = malloc(sizeof(struct guide_split_elem) * cnt);
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

int mac_repair(char *mac, const int length, const char *dot)
{
	int i;
	if (strcmp(dot, ":") == 0){
		for (i=0; i<length; i++){
			if (mac[i] == '-')
				mac[i] = ':';
		}
	}
	else{
		for (i=0; i<length; i++){
			if (mac[i] == ':')
				mac[i] = '-';
		}
	}
    
	return 0;
}

static int guide_divisible(int *val, int val_len, int div)
{
    int i;

    for (i = 0; i < val_len; i++)
    {
        if (val[i] % div != 0)
        {
            break;
        }
    }

    if (i < val_len)
    {
        return (-1);
    }

    for (i = 0; i < val_len; i++)
    {
        val[i] = val[i]/div;
    }

    return 0;
}

void guide_min_ratio(int *val, int val_len)
{
    int div = 2;
    int min_val;
    int i;

    if (val_len <= 0)
    {
        return;
    }
    
    min_val = val[0];
    for (i = 1; i < val_len; i++)
    {
        if (val[i] < min_val)
        {
            min_val = val[i];
        }
    }

    while (div <= min_val)
    {
        if (guide_divisible(val, val_len, div) == 0)
        {
            min_val = min_val/div;
            continue;
        }

        div++;
    }

    return;     
}

int guide_mask_len(unsigned int netmask)
{
    unsigned char *pmask = NULL;
    unsigned char byte;
    int mask_len = 0;
    int i, idx = 0;

    pmask = (unsigned char *)&netmask;
    for (i = 0; i < 4; i++)
    {
        byte = pmask[i];
        if (byte < 0xff)
        {
            break;
        }

        mask_len += 8;
    }

    while (byte != 0)
    {
        byte = byte << 1;
        idx++;
    }

    mask_len += idx;
    return mask_len;
}


