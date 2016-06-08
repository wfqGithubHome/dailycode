#ifndef __GUIDE_ERROR_H__
#define __GUIDE_ERROR_H__

#define GUIDE_OK                                    0
#define GUIDE_UNKNOWN_ERROR                         1
#define GUIDE_READ_FILE_ERROR                       2
#define GUIDE_WRITE_FILE_ERROR                      3
#define GUIDE_MEMORY_ALLOC_FAILED                   4
#define GUIDE_NO_SUCH_MODULE                        5
#define GUIDE_NO_SUCH_ACTION                        6
#define GUIDE_TOO_FEW_PARAMETER                     7 
#define GUIDE_FIRST_LAYER_PARAMETER_PARSE_ERROR     8
#define GUIDE_SECOND_LAYER_PARAMETER_PARSE_ERROR    9



/* related to lan node */
#define GUIDE_LAN_IPADDRESS_ERROR                   21
#define GUIDE_LAN_NETMASK_ERROR                     22
#define GUIDE_LAN_MAC_ADDRESS_ERROR                 23

/* related to wan node */


/* related to adsl node */


/* related to server node */

/* related to zone node */
#define GUIDE_ZONE_NO_ZONE                          101


/* related to dnat node */
#define GUIDE_DANT_ENABLE_ERROR                     121


/* related to submit */
#define GUIDE_SUBMIT_NO_ZONE                        141
#define GUIDE_SUBMIT_BIND_ERROR                     142
#define GUIDE_SUBMIT_INTERNAL_NETWORK_SEGMENT_ERROR 143
#define GUIDE_SUBMIT_LAN_IPADDR_ERROR               144

#endif

