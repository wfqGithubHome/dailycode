#ifndef __GUIDE_UTILS_H__
#define __GUIDE_UTILS_H__

#define MAX_SEGMENT_LENGTH 1024

typedef struct guide_split_elem
{
    char val[MAX_SEGMENT_LENGTH];
} guide_split_elem_t;

int  mac_repair(char *mac, const int length, const char *dot);
int  guide_split(char *input, char *delim, struct guide_split_elem **elems, int *elems_num);
void guide_min_ratio(int *val, int val_len);
int guide_mask_len(unsigned int netmask);



#endif

