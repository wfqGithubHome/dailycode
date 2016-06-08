#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgic.h"
#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

extern void channels_list_show(int channel);
extern void channels_add_show(int channel);
extern void channels_edit_show(int channel);
extern void channels_add_save(int channel);
extern void channels_edit_save(int channel);
extern void channels_remove_save(int channel);
extern void channels_enable_save(void);

void trafficchannels_list_show(void)
{
	channels_list_show(1); // 1:����ͨ����2:����ͨ��
}

void trafficchannels_add_show(void)
{
	channels_add_show(1); // 1:����ͨ����2:����ͨ��
}

void trafficchannels_edit_show(void)
{
	channels_edit_show(1);
}



void trafficchannels_add_save(void)
{
	channels_add_save(1);
}

void trafficchannels_edit_save(void)
{

	channels_edit_save(1);
}

void trafficchannels_remove_save(void)
{
	channels_remove_save(1);
}

void trafficchannels_status_save(void)
{
	channels_enable_save();
}

void trafficchannels_move(void)
{

}


int cgiMain()
{
	char submit_type[STRING_LENGTH];
	char cmd[CMD_LENGTH];
	int iserror=0;

	snprintf(cmd, sizeof(cmd), BACKUP_XMLTMP_CMD);
	system(cmd);

	cgiFormString("submit", submit_type, sizeof(submit_type));
	
#if _MGTCGI_DEBUG_GET_
	printf("content-type:text/html\n\n");		
#else
	printf("content-type:application/json\n\n");	
#endif

	if (strcmp(submit_type,"list") == 0 ){//�б���ʾ����
		trafficchannels_list_show(); 
	}	
	else if(strcmp(submit_type,"add") == 0 ){//������
		trafficchannels_add_show();
	}
	else if(strcmp(submit_type,"edit") == 0){//����༭
		trafficchannels_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//���"ȷ��"
		trafficchannels_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//�༭"ȷ��"
		trafficchannels_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//���ɾ��
		trafficchannels_remove_save();
	}
	else if(strcmp(submit_type,"status") == 0){//���ã�����
		trafficchannels_status_save();
	}
	else if(strcmp(submit_type,"move") == 0){//����
		trafficchannels_move();
	}
	else{
		iserror = MGTCGI_PARAM_ERR;
		goto ERROR_EXIT;
	}
	
	goto CGIC_EXIT;

	ERROR_EXIT:
		printf("{\"iserror\":%d,\"msg\":\"\"}",iserror);
		goto CGIC_EXIT;
	CGIC_EXIT:
		return 0;
}

