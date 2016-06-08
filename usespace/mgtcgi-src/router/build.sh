#!/bin/bash

COMMON_STR="mgtcgi_common.c mgtcgi_check.c mgtcgi_filed_check.c -lcgic -lxmlwrapper4c"

####gcc -o interface.cgi mgtcgi_interface.c ${COMMON_STR}
gcc -o management.cgi mgtcgi_management.c ${COMMON_STR}

gcc -o networks.cgi mgtcgi_networks.c ${COMMON_STR}
gcc -o networkgroups.cgi mgtcgi_networkgroups.c ${COMMON_STR}

gcc -o schedules.cgi mgtcgi_schedules.c ${COMMON_STR}
gcc -o schedulegroups.cgi mgtcgi_schedulegroups.c ${COMMON_STR}

gcc -o vlans.cgi mgtcgi_vlans.c ${COMMON_STR}

gcc -o sessions.cgi mgtcgi_sessions.c ${COMMON_STR}

gcc -o domains.cgi mgtcgi_domains.c ${COMMON_STR}
gcc -o domaingroups.cgi mgtcgi_domaingroups.c ${COMMON_STR}

gcc -o extensions.cgi mgtcgi_extensions.c ${COMMON_STR}
gcc -o extensiongroups.cgi mgtcgi_extensiongroups.c ${COMMON_STR}

gcc -o httpservers.cgi mgtcgi_httpservers.c ${COMMON_STR}
gcc -o httpservergroups.cgi mgtcgi_httpservergroups.c ${COMMON_STR}

gcc -o l7protocols.cgi mgtcgi_layer7protocol.c ${COMMON_STR}

gcc -o customprotocols.cgi mgtcgi_customprotocols.c ${COMMON_STR}
gcc -o customprotogroups.cgi mgtcgi_customprotogroups.c ${COMMON_STR}

gcc -o totalsessions.cgi mgtcgi_totalsessions.c ${COMMON_STR}

gcc -o ipsessions.cgi mgtcgi_ipsessions.c ${COMMON_STR}

gcc -o firewalls.cgi mgtcgi_firewalls.c ${COMMON_STR}

gcc -o trafficchannels.cgi mgtcgi_trafficchannels.c ${COMMON_STR}

gcc -o sharechannels.cgi mgtcgi_sharechannels.c ${COMMON_STR}

gcc -o dnspolicys.cgi mgtcgi_dnspolicys.c ${COMMON_STR}

gcc -o portmirrors.cgi mgtcgi_portmirrors.c ${COMMON_STR}  

gcc -o httpdirpolicys.cgi mgtcgi_httpdirpolicys.c ${COMMON_STR}                       
        
gcc -o configures.cgi mgtcgi_configure.c ${COMMON_STR}

gcc -o route-configure.cgi mgtcgi_route_configure.c mgtcgi_route_xmlparser.c mgtcgi_check.c mgtcgi_common.c mgtcgi_filed_check.c mgtcgi_route_common.c -lcgic -lxmlwrapper4c

gcc -o loadrouteconfig loadroute_xmlconf.c mgtcgi_route_common.c  -lxmlwrapper4c

gcc -o arpbind-update arpbindstatus_update.c -lxmlwrapper4c

gcc -o l7-protoctl l7_protoctl.c -lxmlwrapper4c

gcc -o defroutectl default_route_ctl.c -lxmlwrapper4c
