#ifndef __AC_CACHE_H__
#define __AC_CACHE_H__

#include "ac_report.h"

#define SYSTEM_AC_CACHE_FILE_SYSTEM     "/etc/acfs"
#define DISK_AC_CACHE_FILE_SYSTEM       "/disk/acfs"
#define AC_CACHE_FILE_DIRECTORY_LINK    "/usr/local/httpd/htdocs/cache"

#define AC_CACHE_FILE_SYSTEM_SIZE       128
#define AC_CACHE_URL_LENGTH             512

#define NETLINK_DPI_HTTP_CACHE          42
#define HTTP_CACHE_NETLINK_BASE	        0x30	
#define HTTP_CACHE_NETLINK_ADD          (HTTP_CACHE_NETLINK_BASE + 1)
#define HTTP_CACHE_NETLINK_UPDATE       (HTTP_CACHE_NETLINK_BASE + 2)
#define HTTP_CACHE_NETLINK_DELETE       (HTTP_CACHE_NETLINK_BASE + 3)
#define HTTP_CACHE_NETLINK_FLUSH        (HTTP_CACHE_NETLINK_BASE + 4)
#define HTTP_CACHE_NETLINK_SHOW         (HTTP_CACHE_NETLINK_BASE + 5)
#define HTTP_CACHE_NETLINK_MAX          (HTTP_CACHE_NETLINK_BASE + 6)

#define HTTP_CACHE_DONE                 0
#define HTTP_CACHE_ERROR                1
#define HTTP_CACHE_MEMERR               2
#define HTTP_CACHE_SHOW_DONE            3

#define AC_CACHE_STRING_LENGTH          64

typedef struct acfs_nod
{
    int              fid;
    char             md5sum[MD5SUM_LENGTH];
    char             remote_url[AC_CACHE_URL_LENGTH]; /* ftp://xxx.xxx.xxx.xxx/xxx/xxx.xxx */
    char             local_filename[AC_CACHE_URL_LENGTH]; /* /ac_cache/xxx.xxx */
    int              enable;
} acfs_nod_t;

typedef struct ac_cache_info
{
    uint16_t         id;
    int              fid;
    char             remote_url[AC_CACHE_URL_LENGTH]; /* ftp://xxx.xxx.xxx.xxx/xxx/xxx.xxx */
    char             local_filename[AC_CACHE_URL_LENGTH]; /* /ac_cache/xxx.xxx */
} ac_cache_info_t;

typedef struct ac_cache_respond_info
{
	struct nlmsghdr         nlhdr;
	struct ac_cache_info    info;
} ac_cache_respond_info_t;

extern int ac_cache_file_system_update(struct ac_respond_getapp_unit *units, int unit_count);
extern int ac_cache_context_init(void);
#endif /* __AC_CACHE_H__ */

