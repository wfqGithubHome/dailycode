#ifndef __AC_IP_CLASSIFY_H__
#define __AC_IP_CLASSIFY_H__

#define NETLINK_IP_CLASSIFY           38

#define IP_CLASSIFY_NETLINK_BASE      0x10
#define IP_CLASSIFY_ADD            	  (IP_CLASSIFY_NETLINK_BASE+1)
#define IP_CLASSIFY_SHOW              (IP_CLASSIFY_NETLINK_BASE+2)
#define IP_CLASSIFY_FLUSH          	  (IP_CLASSIFY_NETLINK_BASE+3)
#define IP_CLASSIFY_MATCH             (IP_CLASSIFY_NETLINK_BASE+4)
#define IP_CLASSIFY_LOAD              (IP_CLASSIFY_NETLINK_BASE+5)
#define IP_CLASSIFY_NETLINK_MAX       (IP_CLASSIFY_NETLINK_BASE+6)

#define IP_CLASSIFY_DONE           	  0
#define IP_CLASSIFY_ERROR          	  1
#define IP_CLASSIFY_MEMERR        	  2
#define IP_CLASSIFY_SHOW_DONE 	      3
#define IP_CLASSIFY_MATCH_DONE 	      4

#define IP_CLASSIFY_SHOW_MAX_NODES   100

struct ip_classify_msg
{
	uint32_t  ip_addr;
	uint16_t  masklen;
    uint16_t  dport;
    uint16_t  lay7id;
};

struct ip_classify_respond_info
{
	struct nlmsghdr nlhdr;
	struct ip_classify_msg info[IP_CLASSIFY_SHOW_MAX_NODES];
};


extern int ac_ips_set_to_kernel(uint32_t *ips, int ips_len, uint32_t *sips, int sips_len);


#endif

