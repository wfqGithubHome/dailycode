#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/route_xmlwrapper4c.h"

int main(int argc, char **argv)
{

	int i,j,index,id=0,size=0,errval=1,retval=0;
	char idstr[10]={0};
	struct mgtcgi_xml_arp_binds *arpbind = NULL;
	
	/*if (argc < 2){
		printf("Usage:%s param\n", argv[0]);
		return -1;
	}*/

	get_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ARP_BINDS, (void**)&arpbind, &size);
	if ( NULL == arpbind){
		retval = MGTCGI_READ_FILE_ERR;
		goto ERROR_EXIT;
	}
	//将所有的绑定状态设置为0
	for (index=0; index < arpbind->num; index++){
		arpbind->pinfo[index].status = 0;
	}
	if (argc < 2){
		retval = 0;
		goto SUCCESS_EXIT;
	}

	//将错误ID的状态设置为1
	for (i=0,j=0; i<strlen(argv[1]); i++){
		if(argv[1][i] == ','){
			if (j>0){
				id=(atoi(idstr) - 1);
				//printf("id = %d\n", id);
				if (id < 0){
					retval = -1;
					break;
				}
				if (id  < arpbind->num){
					arpbind->pinfo[id].status = errval;
				}
				else {
					retval = 0;
					break;
				}
				memset(idstr, 0, sizeof(idstr));
			}
			j = 0;
			continue;
		}
		if (j < sizeof(idstr)){
			idstr[j] = argv[1][i];
		}
		else{
			retval = -1;
			break;
		}
		j++;
	}

	//保存
	if (retval == 0 )
		
	SUCCESS_EXIT:
		if (retval ==0 )
			retval=save_route_xml_node(ROUTE_CONFIG_FILE, MGTCGI_TYPE_ARP_BINDS, (void*)arpbind, size);
		goto FREE_EXIT;
	ERROR_EXIT:
		goto FREE_EXIT;
	FREE_EXIT:
		free_xml_node((void*)&arpbind);
		return;
}


