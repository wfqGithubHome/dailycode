#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <opt.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <linux/types.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <link.h>
#include <dlfcn.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include <netdb.h>
#include <ctype.h>
#include <sys/utsname.h>

#include "include/md5.h"
#include "include/utils.h"
#include "include/curl.h"
#include "include/ac_report.h"
#include "include/ac_cache.h"
#include "include/common.h"

#if 1
#define DEBUGP  printf
#else
#define DEBUGP(format,args...)
#endif

#define AC_CACHE_COMMAND_LENGTH     256
#define AC_CACHE_FILE_LINE_BUFFER   1024

struct acfs_nod acfs[AC_CACHE_FILE_SYSTEM_SIZE];
int total = 0;
int right = 0;

size_t write_data(char *buffer, size_t size, size_t nitems, void *outstream)
{
    int written = 0;

    written = fwrite(buffer, size, nitems, (FILE*)outstream); 
    return written;
}

static int md5_check_by_filepath(const char *remote_filepath,char *md5)
{
    int ret = -1;
    int len = 0;
    int fd,i;
    CMD5 remote_md5;
    char remote_filemd5[AC_CACHE_STRING_LENGTH] = {0};
    unsigned char data[AC_CACHE_STRING_LENGTH] = {0};
    unsigned char md5_str[AC_CACHE_STRING_LENGTH] = {0};
    md5[32] = '\0';

    DEBUGP("%s:begin,remote filepath:[%s],md5:[%s]\n",__FUNCTION__,remote_filepath,md5);
    if(remote_filepath == NULL || md5 == NULL){
        goto EXIT;
    }
    
    fd = open(remote_filepath,O_RDONLY);
    if (fd == -1){
        DEBUGP("%s:process,open file [%s] failed\n",__FUNCTION__,remote_filepath);
        goto EXIT;
    }

    while(1){
        len = read(fd,data,AC_CACHE_STRING_LENGTH);
        if(len == -1){
            DEBUGP("%s:process,read file [%s]\n",__FUNCTION__,remote_filepath);
            goto EXIT;
        }

        remote_md5.MD5Update(data,len);
        if(len == 0 || len < AC_CACHE_STRING_LENGTH){
            break;
        }
    }

    close(fd);
    remote_md5.MD5Final(md5_str);

    for(i = 0; i < 16; i++)
    {
        sprintf(remote_filemd5 + i * 2, "%02x", md5_str[i]);
    }
    remote_filemd5[32] = '\0';

    DEBUGP("%s:process,md5:[%s],remote_md5:[%s]\n",__FUNCTION__,md5,remote_filemd5);
    if (memcmp(md5, remote_filemd5, 32) != 0)
    {
        goto EXIT;
    }
    ret = 0;

EXIT:
    DEBUGP("%s:finish,ret:[%d]\n",__FUNCTION__,ret);
    return ret;
}

/*
static int md5_check_by_filefp(FILE *fp,char *md5)
{
    int ret = -1;
    int len = 0;
    int i;
    long int log;
    CMD5 remote_md5;
    char remote_filemd5[AC_CACHE_STRING_LENGTH] = {0};
    unsigned char md5_str[AC_CACHE_STRING_LENGTH] = {0};
    unsigned char data[AC_CACHE_STRING_LENGTH] = {0};
    md5[32] = '\0';

    DEBUGP("%s:begin,fp:[%p],md5:[%s]\n",__FUNCTION__,fp,md5);
    if(fp == NULL || md5 == NULL){
        goto EXIT;
    }
    if(fseek(fp,0,SEEK_SET) != 0){
        DEBUGP("%s:process,fseek failed\n",__FUNCTION__);
    }

    while(1){
        len = fread(data,AC_CACHE_STRING_LENGTH,1,fp);
        remote_md5.MD5Update(data,len);
        if(len == 0 || len < (AC_CACHE_STRING_LENGTH - 1)){
            break;
        }
        memset(data,0,sizeof(data));
    }
    
    remote_md5.MD5Final(md5_str);
    for(i = 0; i < 16; i++)
    {
        sprintf(remote_filemd5 + i * 2, "%02X", md5_str[i]);
    }

    remote_filemd5[32] = '\0';
    DEBUGP("%s:process,md5:[%s],remote_md5:[%s]\n",__FUNCTION__,md5,remote_filemd5);
    if (memcmp(md5, remote_filemd5, 32) != 0)
    {
        goto EXIT;
    }
    ret = 0;

EXIT:
    DEBUGP("%s:finish,ret:[%d]\n",__FUNCTION__,ret);
    return ret;
}*/

static struct acfs_nod * ac_cache_file_system_free_nod_get(void)
{
    struct acfs_nod * nod = NULL;
    int idx;

    for (idx=0; idx<AC_CACHE_FILE_SYSTEM_SIZE; idx++)
    {
        if(acfs[idx].enable == 0)
        {
            nod = &acfs[idx];
            break;
        }
    }

    return nod;
}

static struct acfs_nod * ac_cache_file_system_nod_find(int fid)
{
    struct acfs_nod * nod = NULL;
    int idx;

    for (idx=0; idx<AC_CACHE_FILE_SYSTEM_SIZE; idx++)
    {
        if (acfs[idx].enable == 0)
        {
            continue;
        }
        
        if(acfs[idx].fid == fid)
        {
            nod = &acfs[idx];
            break;
        }
    }

    return nod;
}   

int ac_cache_file_system_forward(int fid, char *remote_url, char *local_filename, int op)
{
    int                     sock;
	int                     recv_len;
    struct ac_cache_info    request;
	int                     respond_size;
	void                    *respond;
	struct msghdr           msg;
	struct iovec            iov;
	struct sockaddr_nl      remote;
	struct sockaddr_nl      local;
	struct nlmsghdr         * nlhdr;
	struct nlmsghdr         * rnlhdr=NULL;
	socklen_t               remote_addr_len;
    int                     res, ret = -1;

	sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_DPI_HTTP_CACHE); 
	if(sock < 0)
    {
		DEBUGP("create socket error\n");
		return ret;
	}

    memset(&request, 0, sizeof(request));
    request.fid = fid;
    strncpy(request.remote_url, remote_url, AC_CACHE_URL_LENGTH-1);
    strncpy(request.local_filename, local_filename, AC_CACHE_URL_LENGTH-1);
    
	memset(&local, 0, sizeof(struct sockaddr_nl));
	local.nl_family = AF_NETLINK;
	local.nl_pid = getpid();
	local.nl_groups = 0; 

	if(bind(sock, (struct sockaddr*)&local, sizeof(struct sockaddr_nl)) != 0)
    {
		DEBUGP("bind socket error\n");
		goto exit;
	}

	memset(&remote, 0, sizeof(struct sockaddr_nl));
	remote.nl_family = AF_NETLINK;
	remote.nl_pid    = 0;
	remote.nl_groups = 0;

	memset(&msg, 0, sizeof(struct msghdr));

	nlhdr = (struct nlmsghdr *)malloc(NLMSG_SPACE(sizeof(struct ac_cache_info)));

	memcpy(NLMSG_DATA(nlhdr), (char *)&request, sizeof(struct ac_cache_info));

	nlhdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct ac_cache_info));
	nlhdr->nlmsg_pid = getpid();  
	nlhdr->nlmsg_flags = NLM_F_REQUEST;
	nlhdr->nlmsg_type = op;

	iov.iov_base = (void *)nlhdr;
	iov.iov_len = nlhdr->nlmsg_len;

	msg.msg_name = (void *)&remote;
	msg.msg_namelen = sizeof(remote);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	res = sendmsg(sock, &msg, 0);
	if (res == -1) 
    {
		 DEBUGP("sendmsg error\n");
		 goto exit;
	}

	remote_addr_len = sizeof(struct sockaddr_nl);
	respond_size    = sizeof(struct ac_cache_respond_info);
	respond         = malloc(respond_size);

	memset(respond, 0, respond_size);
	recv_len = recvfrom(sock, respond, respond_size,
	       0, (struct sockaddr*)&remote, &remote_addr_len);

	if(recv_len == -1)
    {
		DEBUGP("recvmsg error\n");
		goto exit;
	}

	rnlhdr = (struct nlmsghdr *)respond;
	switch(rnlhdr->nlmsg_type)
    {
		case HTTP_CACHE_DONE:
            ret = 0;
			break;
        
		default:
			break;
	}

exit:
	close(sock);
	if(respond)
	{
		free(respond);
	}
    
	if(nlhdr)
	{
		free(nlhdr);
	}

	return  ret;
}

int ac_cache_file_system_save(void)
{
    FILE                *fp = NULL;
    char                *linebuf = NULL;
    char                cmd[AC_CACHE_COMMAND_LENGTH] = {0};
    struct acfs_nod     *nod = NULL;
    int                 idx, ret = -1;
    
    DEBUGP("%s: begin\n", __FUNCTION__);
    
    linebuf = (char *)malloc(AC_CACHE_FILE_LINE_BUFFER);
    if (linebuf == NULL)
    {
        printf("%s: alloc line buffer failed\n", __FUNCTION__);
        goto exit;
    }
    
    memset(linebuf, 0, AC_CACHE_FILE_LINE_BUFFER);
    fp = fopen(SYSTEM_AC_CACHE_FILE_SYSTEM, "wt+");
    if (fp == NULL)
    {
        printf("%s: can not open %s\n", __FUNCTION__, SYSTEM_AC_CACHE_FILE_SYSTEM);
        goto exit;
    }

    for (idx=0; idx<AC_CACHE_FILE_SYSTEM_SIZE; idx++)
    {
        nod = &acfs[idx];
        if (nod->enable == 0)
        {
            continue;
        }

        snprintf(linebuf, AC_CACHE_FILE_LINE_BUFFER, "%d %s %s %s\n", 
            nod->fid, nod->md5sum, nod->remote_url, nod->local_filename);
        fputs(linebuf, fp);
    }
    
    fclose(fp);
    snprintf(cmd, AC_CACHE_COMMAND_LENGTH, "cp -f %s %s >/dev/null 2>&1", 
        SYSTEM_AC_CACHE_FILE_SYSTEM, DISK_AC_CACHE_FILE_SYSTEM);
    system(cmd);
    ret = 0;
exit:
    if (linebuf)
    {
        free(linebuf);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

int ac_cache_file_system_load(void)
{
    FILE                * fp = NULL;
    char                * linebuf = NULL;
    struct acfs_nod     * nod = NULL;
    struct split_elem   * elems = NULL;
    int                 elems_num = 0;
    int                 idx = 0, ret = -1;
    
    DEBUGP("%s: begin\n", __FUNCTION__);

    linebuf = (char *)malloc(AC_CACHE_FILE_LINE_BUFFER);
    if (linebuf == NULL)
    {
        printf("%s: alloc line buffer failed\n", __FUNCTION__);
        goto exit;
    }

    memset(linebuf, 0, AC_CACHE_FILE_LINE_BUFFER);
    fp = fopen(SYSTEM_AC_CACHE_FILE_SYSTEM, "r");
    if (fp == NULL)
    {
        printf("%s: can not open ac file system: %s\n", __FUNCTION__, SYSTEM_AC_CACHE_FILE_SYSTEM);
        goto exit;
    }

    /* line info: fid major_version minor_version remote_url local_filename */
    while(fgets(linebuf, AC_CACHE_FILE_LINE_BUFFER, fp) != NULL)
    {
        if (idx >= AC_CACHE_FILE_SYSTEM_SIZE)
        {
            break;
        }
        
        if (split(linebuf, " ", &elems, &elems_num) != 0)
        {
            continue;
        }

        if (elems == NULL || elems_num != 4)
        {
            continue;
        }

        nod = &acfs[idx++];
        nod->fid = atoi(elems[0].val);
        strncpy(nod->md5sum, elems[1].val, MD5SUM_LENGTH-1);
        strncpy(nod->remote_url, elems[2].val, AC_CACHE_URL_LENGTH-1);
        strncpy(nod->local_filename, elems[3].val, AC_CACHE_URL_LENGTH-1);
        nod->enable = 1;
        DEBUGP("%s: idx: %d, fid: %d, md5sum: %s, url: %s, filename: %s, enable: %d\n", 
            __FUNCTION__, idx-1, nod->fid, nod->md5sum, nod->remote_url, 
            nod->local_filename, nod->enable);
    }

    ret = 0;
exit:
    if (elems)
    {
        free(elems);
    }
    
    if (linebuf)
    {
        free(linebuf);
    }

    if (fp)
    {
        fclose(fp);
    }
    
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}

void ac_cache_file_system_unload(void)
{
    return;
}

void ac_cache_file_delete(int fid)
{
    struct acfs_nod *nod = NULL;
    char cmd[AC_CACHE_COMMAND_LENGTH] = {0};

    DEBUGP("%s: begin, fid: %d\n", __FUNCTION__, fid);

    nod = ac_cache_file_system_nod_find(fid);

    if (nod == NULL)
    {
        return;
    }
    
    snprintf(cmd, AC_CACHE_COMMAND_LENGTH, "rm -f %s/%s 2>/dev/null", 
        AC_CACHE_FILE_DIRECTORY_LINK, nod->local_filename);
    system(cmd);

    DEBUGP("%s: finish\n", __FUNCTION__);
    return;
}

int ac_cache_file_get(char *remote_url, char *local_filename)
{
    CURL *pcurl = NULL;
    FILE *fp = NULL;
    char filepath[AC_CACHE_URL_LENGTH] = {0};
    int ret = -1;

    DEBUGP("%s: begin, remote_url: %s, local_filename: %s\n", 
        __FUNCTION__, remote_url, local_filename);

    pcurl = curl_easy_init(); 
    snprintf(filepath, AC_CACHE_URL_LENGTH, "%s/%s", AC_CACHE_FILE_DIRECTORY_LINK, local_filename);
    fp = fopen(filepath, "wt+"); 
    if (fp == NULL)
    {
        printf("%s: can not open or create %s\n", __FUNCTION__, filepath);
        goto exit;
    }
    
    curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, (void*)fp );
    curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, write_data); 
    curl_easy_setopt(pcurl, CURLOPT_URL, remote_url); 
    curl_easy_perform(pcurl);
    fclose(fp);

    curl_easy_cleanup( pcurl );
    ret = 0;
exit:
    DEBUGP("%s: finish\n", __FUNCTION__);
    return ret;
}   

static void __ac_cache_file_system_update(int fid, char *md5sum, 
    char *remote_url, char *local_filename, int status, int *dirty)
{
    char            download_url[AC_CACHE_URL_LENGTH] = {0};
    struct acfs_nod *nod = NULL;
    
    DEBUGP("%s: begin, fid: %d, md5sum: %s, url: %s, filename: %s, status: %d\n", 
        __FUNCTION__, fid, md5sum, remote_url, local_filename, status);

    nod = ac_cache_file_system_nod_find(fid);

    /* not found in list, this is a new cache item*/
    if (nod == NULL)
    {
        /* invalid item, have removed from list */
        if (status == 1)
        {
            return;
        }

        nod = ac_cache_file_system_free_nod_get();
        if (nod == NULL)
        {
            return;
        }

        snprintf(download_url, AC_CACHE_URL_LENGTH, "%s%s", "http://", remote_url);
        ac_cache_file_get(download_url, local_filename);
        nod->fid = fid;
        strncpy(nod->md5sum, md5sum, MD5SUM_LENGTH-1);
        strncpy(nod->remote_url, remote_url, AC_CACHE_URL_LENGTH-1);
        strncpy(nod->local_filename, local_filename, AC_CACHE_URL_LENGTH-1);
        nod->enable = 1;
        ac_cache_file_system_forward(fid, remote_url, local_filename, HTTP_CACHE_NETLINK_ADD);
        *dirty = 1;
        return;
    }

    /* should moved a item from list */
    if (status == 1)
    {
        ac_cache_file_delete(fid);
        ac_cache_file_system_forward(fid, remote_url, local_filename, HTTP_CACHE_NETLINK_DELETE);
        nod->enable = 0;
        *dirty = 1;
        return;
    }

    /* update a cache item */
    if (strcmp(nod->md5sum, md5sum) != 0)
    {
        ac_cache_file_delete(fid);
        snprintf(download_url, AC_CACHE_URL_LENGTH, "%s%s", "http://", remote_url);
        ac_cache_file_get(download_url, local_filename);
        ac_cache_file_system_forward(fid, remote_url, local_filename, HTTP_CACHE_NETLINK_UPDATE);
        strncpy(nod->md5sum, md5sum, MD5SUM_LENGTH-1);
        strncpy(nod->remote_url, remote_url, AC_CACHE_URL_LENGTH-1);
        strncpy(nod->local_filename, local_filename, AC_CACHE_URL_LENGTH-1);
        *dirty = 1;
    }
    return;
}

int ac_cache_file_system_update(struct ac_respond_getapp_unit *units, 
    int unit_count)
{
    int             acfs_dirty = 0;
    int             idx, ret = 0;

    DEBUGP("%s: begin, unit_count: %d\n", __FUNCTION__, unit_count);

    memset(&acfs, 0, sizeof(struct acfs_nod) * AC_CACHE_FILE_SYSTEM_SIZE);

    /* read cache file info from /etc/acfs */
    ac_cache_file_system_load();
    for (idx=0; idx<unit_count; idx++)
    {
        struct ac_respond_getapp_unit *unit = &units[idx];
        char                          remote_url[AC_CACHE_URL_LENGTH] = {0};
        char                          filename[AC_CACHE_URL_LENGTH] = {0};
        char                          * sep = NULL;
        
        sep = strstr(unit->app_url, "http://");
        if (sep == NULL)
        {
            continue;
        }

        sep += strlen("http://");
        strncpy(remote_url, sep, AC_CACHE_URL_LENGTH-1);

        sep = str_reverse_char(unit->app_url, '/');
        if (sep == NULL  || *(sep+1) == '\0')
        {
            continue;
        }

        strncpy(filename, sep+1, AC_CACHE_URL_LENGTH-1);
        
        __ac_cache_file_system_update(unit->app_id, unit->md5sum, remote_url, 
            filename, unit->status, &acfs_dirty);
    }
    
    if (acfs_dirty == 1)
    {
        ac_cache_file_system_save();
    }
    
    ac_cache_file_system_unload();
    DEBUGP("%s: finish, ret: %d\n", __FUNCTION__, ret);
    return ret;
}


int ac_cache_context_init(void)
{
    struct acfs_nod *nod = NULL;
    int idx;

    DEBUGP("%s: begin\n", __FUNCTION__);

    system("/usr/local/sbin/ac_cache_context_init.sh >/dev/null 2>&1");
    memset(&acfs, 0, sizeof(struct acfs_nod) * AC_CACHE_FILE_SYSTEM_SIZE);
    ac_cache_file_system_load();
    for (idx=0; idx<AC_CACHE_FILE_SYSTEM_SIZE; idx++)
    {
        nod = &acfs[idx];
        if (nod->enable == 0)
        {
            continue;
        }

        ac_cache_file_system_forward(nod->fid, nod->remote_url, nod->local_filename, 
            HTTP_CACHE_NETLINK_ADD);
    }
    
    ac_cache_file_system_unload();
    return 0;
}

int ac_cache_webfile_get(char *remote_url, char *local_filename, char *local_md5sum)
{
    CURL *pcurl = NULL;
    FILE *fp = NULL;
    char filepath[AC_CACHE_URL_LENGTH] = {0};
    int ret = -1;
    CURLcode res;
    int num = 3;
    
    DEBUGP("%s: begin, remote_url: %s, local_filename: %s ,local_md5sum: %s\n", 
        __FUNCTION__, remote_url, local_filename, local_md5sum);

    pcurl = curl_easy_init(); 
    snprintf(filepath, AC_CACHE_URL_LENGTH, "%s%s", DEVICE_WIFISEVER_ROOTPAHT, local_filename);
#if 0
    snprintf(cmd, AC_CACHE_COMMAND_LENGTH, "wget -O %s %s >/dev/null 2>&1", 
                 filepath,remote_url);
    
    system(cmd);
#endif
    total++;
    while(num--){
        fp = fopen(filepath, "wt+"); 
        if (fp == NULL)
        {
            goto exit;
        }
        curl_easy_setopt(pcurl, CURLOPT_WRITEDATA, (void*)fp );
        curl_easy_setopt(pcurl, CURLOPT_WRITEFUNCTION, write_data); 
        curl_easy_setopt(pcurl, CURLOPT_URL, remote_url); 
        curl_easy_setopt(pcurl, CURLOPT_TIMEOUT, 600 );
        curl_easy_setopt(pcurl, CURLOPT_CONNECTTIMEOUT, 10 );
        res = curl_easy_perform(pcurl);
        printf("%s: res:%d\n", __FUNCTION__, res);
        fclose(fp);
        if (md5_check_by_filepath(filepath,local_md5sum) == 0){
            ret = 0;
            right++;
            break;
        }
    }

    curl_easy_cleanup( pcurl );
exit:
    DEBUGP("%s: finish\n", __FUNCTION__);
    return ret;
}

int __ac_webfile_get_fileunit(FILE *fp, struct ac_respond_webfile_unit *punit)
{
    int ret = -1;
    char linebuf[AC_CACHE_FILE_LINE_BUFFER]={0};
    struct split_elem   * elems = NULL;
    int elems_num;
    
    if(!fp)
    {
        return ret;
    }
    if(fgets(linebuf, AC_CACHE_FILE_LINE_BUFFER, fp) == NULL){
        return ret;
    }
    
    if (split(linebuf, "|", &elems, &elems_num) != 0)
    {
        return ret;
    }
    
    if (elems == NULL || elems_num != 3)
    {
        return ret;
    }
    
    strncpy(punit->file_path, elems[0].val, APP_NAME_LENGTH-1);
    snprintf(punit->file_name,WEBFILE_NAME_LENGTH,"%s%s",elems[0].val,elems[1].val);
    strncpy(punit->md5sum, elems[2].val, MD5SUM_LENGTH-1);
    DEBUGP("%s: md5sum: %s, filename: %s, file_path: %s\n", 
        __FUNCTION__, punit->md5sum, punit->file_name, punit->file_path);
    if (elems)
    {
        free(elems);
    }
    ret = 0;
    return ret;
}

int ac_webfile_system_update(char *webfileinfo, char *devicewebfileinfo)
{
    FILE *remotefp = NULL, *localfp=NULL;
    int ret = -1;
    //int i;
    char linebuf[AC_CACHE_FILE_LINE_BUFFER] = {0};
    struct ac_respond_webfile_unit remoteunit,localunit;
    struct ac_respond_webfile_info remoteinfo,localinfo;
    int cmp=0;
    char cmd[AC_CACHE_COMMAND_LENGTH] = {0}, remoteurl[AC_CACHE_COMMAND_LENGTH]={0};
    printf("%s: webfileinfo=%s,devicefileinfo=%s\n",__FUNCTION__,webfileinfo,devicewebfileinfo);
    remotefp = fopen(webfileinfo, "r");
    if(!remotefp)
    {
        return ret;
    }
    localfp=fopen(devicewebfileinfo, "r");
    memset(&remoteinfo,0,sizeof(struct ac_respond_webfile_info));
    memset(&localinfo,0,sizeof(struct ac_respond_webfile_info));
    printf("%s: file=%p,file=%p\n",__FUNCTION__,remotefp,localfp);
    if(localfp){
        fgets(linebuf, AC_CACHE_FILE_LINE_BUFFER, localfp);
        if(strlen(linebuf) > 0)
            localinfo.version = atoi(linebuf);
        fgets(linebuf, AC_CACHE_FILE_LINE_BUFFER, localfp);
        strncpy(localinfo.getfile_url,linebuf,strlen(linebuf) - 1);
    }
    memset(linebuf,0,AC_CACHE_FILE_LINE_BUFFER);
    fgets(linebuf, AC_CACHE_FILE_LINE_BUFFER, remotefp);
    remoteinfo.version = atoi(linebuf);
    
    memset(linebuf, 0, AC_CACHE_FILE_LINE_BUFFER);
    fgets(linebuf, AC_CACHE_FILE_LINE_BUFFER, remotefp);
    strncpy(remoteinfo.getfile_url,linebuf,strlen(linebuf)-1);
    
    //strcpy(remoteinfo.getfile_url,"http://ad.sunnywifi.com/downloadv2");
    //printf("%s: version=%d,oldversion=%d url=%s\n",__FUNCTION__,remoteinfo.version,localinfo.version,remoteinfo.getfile_url);
    if(remoteinfo.version == localinfo.version){
        DEBUGP("%s: remoteinfo.version: %d, localinfo.version: %d\n", __FUNCTION__,
                    remoteinfo.version, localinfo.version);
        goto EXIT;
    }
    ret = 0;
    do
    {
        memset(&remoteunit,0,sizeof(struct ac_respond_webfile_unit));
        __ac_webfile_get_fileunit(remotefp,&remoteunit);
        do
        {
            memset(&localunit,0,sizeof(struct ac_respond_webfile_unit));
            __ac_webfile_get_fileunit(localfp,&localunit);
            if(localunit.file_name[0]==0){
                cmp = 1;
                break;
            }
            //printf("%s: file_name=%s,md5sum=%s url=%s\n",__FUNCTION__,localunit.file_name,localunit.md5sum,remoteinfo.getfile_url);
            if(remoteunit.file_name[0]!=0){
                cmp = strcmp(localunit.file_name,remoteunit.file_name);
                if(cmp < 0)
                {
                    snprintf(cmd, AC_CACHE_COMMAND_LENGTH, "rm -f %s%s >/dev/null 2>&1", 
                            DEVICE_WIFISEVER_ROOTPAHT, localunit.file_name);
                    system(cmd);
                }
            }
            else if(localunit.file_name[0]!=0 ){
                cmp = -1;
                snprintf(cmd, AC_CACHE_COMMAND_LENGTH, "rm -f %s%s >/dev/null 2>&1", 
                            DEVICE_WIFISEVER_ROOTPAHT, localunit.file_name);
                system(cmd);
            }
        }while(localunit.file_name[0]!=0 && cmp<0);
        
        //printf("%s: file_name=%s,md5sum=%s url=%s\n",__FUNCTION__,remoteunit.file_name,remoteunit.md5sum,remoteinfo.getfile_url);
        if(cmp > 0 && remoteunit.file_name[0]!=0)
        {
            snprintf(cmd, AC_CACHE_COMMAND_LENGTH, "mkdir -p %s%s >/dev/null 2>&1", 
                        DEVICE_WIFISEVER_ROOTPAHT, remoteunit.file_path);
            system(cmd);
            snprintf(remoteurl, AC_CACHE_COMMAND_LENGTH, "%s%s", 
                        remoteinfo.getfile_url, remoteunit.file_name);
            ret = ac_cache_webfile_get(remoteurl, remoteunit.file_name, remoteunit.md5sum);
            if(ret != 0)
                break;
            //check md5sum  add .............
            
        }
    }while(remoteunit.file_name[0]!=0||localunit.file_name[0]!=0);
    
EXIT:
    if(remotefp)
    {
        fclose(remotefp);
    }
    if(localfp)
    {
        fclose(localfp);
    }
    return ret;
}



