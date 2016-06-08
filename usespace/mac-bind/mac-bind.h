#ifndef _IP_MAC_H_
#define _IP_MAC_H_

#define IP_MAC_LONG_NAME        		"Mac-Bind"
#define IP_MAC_NAME                   		"Mac-Bind"
#define IP_MAC_DESCRIPTION      		"IP-MAC-BIND"
#define IP_MAC_VERSION             		"1.0.0"

#define DEV_ERR                            		-1 
#define DEV_OK						0

#define NETLINK_IP_MAC                	32
#define MAC_BINDING_NETLINK_BASE  0x10
#define MAC_BINDING_ADD            		(MAC_BINDING_NETLINK_BASE+1)
#define MAC_BINDING_DELETE         	(MAC_BINDING_NETLINK_BASE+2)
#define MAC_BINDING_SHOW            	(MAC_BINDING_NETLINK_BASE+3)
#define MAC_BINDING_ACTION         	(MAC_BINDING_NETLINK_BASE+4)
#define MAC_BINDING_GET_ACTION      (MAC_BINDING_NETLINK_BASE+5)
#define MAC_BINDING_CHANGE          	(MAC_BINDING_NETLINK_BASE+6)
#define MAC_BINDING_FLUSH          	(MAC_BINDING_NETLINK_BASE+7)
#define MAC_BINDING_NETLINK_MAX    (MAC_BINDING_NETLINK_BASE+8)

#define MAC_BAND_FLAG          1

//return info
#define MAC_BINDING_DONE           	0
#define MAC_BINDING_ERROR          	1
#define MAC_BINDING_MEMERR        	2
#define MAC_BINDING_SHOW_DONE 	    3
#define MAC_BINDING_GET_DONE 		4

#define SHOW_MAX_NODES            	100
#define NAME_MAX_LEN		      	32
#define IP_MAC_ADDR_MIN_LEN    		6
#define MAC_LENGTH                  6

#define MAC_ADDR_MIN		      	0
#define MAC_ADDR_MAX                255

#define NOT_MATCH_ACCEPT         	1
#define NOT_MATCH_DROP              0

struct mac_bind_config_info
{
	char *name;
	char *ip_addr;
	char *mac_addr;
	char *action;
	int     add;
	int     del;
	int     show;
    int     cgishow;
	int     change;
	int     get;
	int     set;
	int     flush;
	int     bridge;
       int     page;
       int     page_size;
	int     help;
};

struct mac_bind_msg
{
	uint32_t        ip_addr;
    uint16_t        bridge_id;
	unsigned char   mac_addr[MAC_LENGTH];
	char            name[NAME_MAX_LEN];
	uint8_t         action;
    
    uint32_t        last_uptime;
    uint64_t        in_bytes;
    uint64_t        out_bytes;
};

struct mac_bind_respond_info
{
	struct nlmsghdr nlhdr;
	struct mac_bind_msg info[SHOW_MAX_NODES];
};

#endif
