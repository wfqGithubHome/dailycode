#ifndef __COMMON_H__
#define __COMMON_H__

#define MD5KEY "1234~!@#$%^&*()QWERTP{}!@#$%^$@"

void ten_second_delay(void);
int device_hwid_read(unsigned char *hwid);
void get_timestamp(time_t *timestamp);

#endif
