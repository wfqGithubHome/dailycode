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

void sharechannels_list_show(void)
{
	channels_list_show(2); // 1:����ͨ����2:����ͨ��
}

void sharechannels_add_show(void)
{
	channels_add_show(2); // 1:����ͨ����2:����ͨ��
}

void sharechannels_edit_show(void)
{
	channels_edit_show(2);
}


void sharechannels_add_save(void)
{
	channels_add_save(2);// 1:����ͨ����2:����ͨ��
}

void sharechannels_edit_save(void)
{
	channels_edit_save(2);
}

void sharechannels_remove_save(void)
{	
	channels_remove_save(2);
}

void sharechannels_status_save(void)
{
	channels_enable_save();
}

void sharechannels_move(void)
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
		sharechannels_list_show();
	}	
	else if(strcmp(submit_type,"add") == 0 ){//������
		sharechannels_add_show(); 
	}
	else if(strcmp(submit_type,"edit") == 0){//����༭
		sharechannels_edit_show();
	}
	else if(strcmp(submit_type,"addsave") == 0){//���"ȷ��"
		sharechannels_add_save();
	}
	else if(strcmp(submit_type,"editsave") == 0){//�༭"ȷ��"
		sharechannels_edit_save();
	}
	else if(strcmp(submit_type,"remove") == 0){//���ɾ��
		sharechannels_remove_save();
	}
	else if(strcmp(submit_type,"status") == 0){//���ã�����
		sharechannels_status_save();
	}
	else if(strcmp(submit_type,"move") == 0){//����
		sharechannels_move();
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

