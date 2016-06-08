#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

#define SERVER_INFO_FILENAME    "/tmp/servinfo.sh"

#if 0
#define DEBUGP printf
#else
#define DEBUGP(format, args...)
#endif


int main(int argc, char **argv)
{
    FILE *fp = NULL;
    struct mgtcgi_xml_servinfo *servinfo = NULL;
    int servinfo_size;
    char rowbuf[512] = {"0"};
    int i, j=0;
	int ret = 0;

    DEBUGP("%s: begin\n", __FUNCTION__);
    
    fp = fopen(SERVER_INFO_FILENAME, "wt+");
    if (fp == NULL)
    {
        fprintf(stderr, "can not open or create %s\n", SERVER_INFO_FILENAME);
        ret = -1;
        goto exit;
    }
    
	get_xml_nodeforservinfo(MGTCGI_XMLTMP,SERV_XMLFILE, (void**)&servinfo,&servinfo_size);
	if (servinfo == NULL){
		ret = 0;
        snprintf(rowbuf,512,"var1_1=http://ngtung.tu-e.cn/os/api.php\n");
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var2_1=""\n");
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var3_1=""\n");
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var4_1=""\n");
        fputs(rowbuf, fp);
		goto exit;
	}

    for (i = 0; i < servinfo->num; i++)
    {
        if (servinfo->pinfo[i].selected == 0)
        {
            continue;
        }
        j++;
        snprintf(rowbuf, 512, "var1_%u=%s\n", servinfo->pinfo[i].type, servinfo->pinfo[i].var1);
        fputs(rowbuf, fp);       
        snprintf(rowbuf, 512, "var2_%u=%s\n", servinfo->pinfo[i].type, servinfo->pinfo[i].var2);
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var3_%u=%s\n", servinfo->pinfo[i].type, servinfo->pinfo[i].var3);
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var4_%u=%s\n", servinfo->pinfo[i].type, servinfo->pinfo[i].var4);
        fputs(rowbuf, fp);
    }
    
    if(j==0)
    {
        snprintf(rowbuf,512,"var1_1=http://ngtung.tu-e.cn/os/api.php\n");
        fputs(rowbuf, fp);       
        snprintf(rowbuf, 512, "var2_%u=%s\n", servinfo->pinfo[1].type, servinfo->pinfo[1].var2);
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var3_%u=%s\n", servinfo->pinfo[1].type, servinfo->pinfo[1].var3);
        fputs(rowbuf, fp);
        snprintf(rowbuf, 512, "var4_%u=%s\n", servinfo->pinfo[1].type, servinfo->pinfo[1].var4);
        fputs(rowbuf, fp);
    }
exit:
    if (servinfo)
    {
	    free_xml_node((void **)&servinfo);
    }
    
    if (fp)
    {
        fclose(fp);
    }

    DEBUGP("%s: finish, ret = %d\n", __FUNCTION__, ret);
	return ret;
}

