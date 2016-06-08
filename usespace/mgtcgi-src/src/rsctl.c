#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/xmlwrapper4c.h"
#include "../include/mgtcgi_xmlconfig.h"

int change_passwd(char *action)
{
    int iserror = -1,size,ret_value;
    struct mgtcgi_xml_system *system = NULL;

    if(!action){
        goto ERROR_EXIT;
    }
    
    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_SYSTEM, (void**)&system, &size); 
    if(system == NULL){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    strncpy(system->password2,action,STRING_LENGTH-1);

    ret_value = save_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_SYSTEM,(void *)system, size);
    if(ret_value < 0){
        ret_value = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }
    iserror = 0;

ERROR_EXIT:
    if(system){
        free_xml_node((void *)&system);
    }
    return iserror;
}

int enable_passwd(char *action)
{
    int iserror = -1,size,ret_value;
    struct mgtcgi_xml_routeserver *mgt = NULL;

    if(!action){
        goto ERROR_EXIT;
    }

    get_xml_node(MGTCGI_XMLTMP, MGTCGI_TYPE_ROUTESERVICE, (void**)&mgt, &size); 
    if(mgt == NULL){
        iserror = MGTCGI_READ_FILE_ERR;
        goto ERROR_EXIT;
    }

    mgt->passwd_enable = (atoi(action))?1:0;

    ret_value = save_xml_node(MGTCGI_XMLTMP,MGTCGI_TYPE_ROUTESERVICE,(void *)mgt, size);
    if(ret_value < 0){
        ret_value = MGTCGI_WRITE_FILE_ERR;
        goto ERROR_EXIT;
    }
    iserror = 0;

ERROR_EXIT:
    if(mgt){
        free_xml_node((void *)&mgt);
    }
    return iserror;

}

static void Print_usage(void)
{
    fprintf(stderr,"Usage: rsctl [ option ] ACTION\n");
    fprintf(stderr,"The option are:\n");
    fprintf(stderr,"    pwdcge --change password\n");
    fprintf(stderr,"    pwdenable --enable password\n");
    fprintf(stderr,"    help --print help info\n");
}

int main(int argc,char *argv[])
{
    char option[STRING_LENGTH] = {0};
    char action[STRING_LENGTH] = {0};
    
    if(argc <= 2){
        Print_usage();
        return 0;
    }

    strcpy(option,argv[1]);
    strcpy(action,argv[2]);

    if(strcmp(option,"pwdcge") == 0){
        change_passwd(action);
    }
    else if(strcmp(option,"pwdenable") == 0){
        enable_passwd(action);
    }
    else{
        Print_usage();
    }

    return 0;
}
