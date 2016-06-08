#!/bin/bash

COMMON_STR="mgtcgi_common.c mgtcgi_check.c mgtcgi_filed_check.c -L. -lcgic -lxmlwrapper4c"

####gcc -o interface.cgi mgtcgi_interface.c ${COMMON_STR}
gcc -Wall -o management.cgi mgtcgi_management.c ${COMMON_STR}

gcc -Wall -o networks.cgi mgtcgi_networks.c ${COMMON_STR}
gcc -Wall -o networkgroups.cgi mgtcgi_networkgroups.c ${COMMON_STR}

gcc -Wall -o schedules.cgi mgtcgi_schedules.c ${COMMON_STR}
gcc -Wall -o schedulegroups.cgi mgtcgi_schedulegroups.c ${COMMON_STR}

gcc -Wall -o vlans.cgi mgtcgi_vlans.c ${COMMON_STR}

gcc -Wall -o sessions.cgi mgtcgi_sessions.c ${COMMON_STR}

gcc -Wall -o domains.cgi mgtcgi_domains.c ${COMMON_STR}
gcc -Wall -o domaingroups.cgi mgtcgi_domaingroups.c ${COMMON_STR}

gcc -Wall -o extensions.cgi mgtcgi_extensions.c ${COMMON_STR}
gcc -Wall -o extensiongroups.cgi mgtcgi_extensiongroups.c ${COMMON_STR}

gcc -Wall -o httpservers.cgi mgtcgi_httpservers.c ${COMMON_STR}
gcc -Wall -o httpservergroups.cgi mgtcgi_httpservergroups.c ${COMMON_STR}

gcc -Wall -o l7protocols.cgi mgtcgi_layer7protocol.c ${COMMON_STR}

gcc -Wall -o customprotocols.cgi mgtcgi_customprotocols.c ${COMMON_STR}
gcc -Wall -o customprotogroups.cgi mgtcgi_customprotogroups.c ${COMMON_STR}

gcc -Wall -o totalsessions.cgi mgtcgi_totalsessions.c ${COMMON_STR}

gcc -Wall -o ipsessions.cgi mgtcgi_ipsessions.c ${COMMON_STR}

gcc -Wall -o firewalls.cgi mgtcgi_firewalls.c ${COMMON_STR}

gcc -Wall -o trafficchannels.cgi mgtcgi_trafficchannels.c ${COMMON_STR}

gcc -Wall -o sharechannels.cgi mgtcgi_sharechannels.c ${COMMON_STR}

gcc -Wall -o dnspolicys.cgi mgtcgi_dnspolicys.c ${COMMON_STR}

gcc -Wall -o portmirrors.cgi mgtcgi_portmirrors.c ${COMMON_STR}  

gcc -Wall -o httpdirpolicys.cgi mgtcgi_httpdirpolicys.c ${COMMON_STR}         

gcc -Wall -o timing.cgi mgtcgi_timing.c ${COMMON_STR}

gcc -Wall -o wifi.cgi mgtcgi_wifi.c ${COMMON_STR}

gcc -Wall -o firstgamechannel.cgi mgtcgi_firstgamechannel.c ${COMMON_STR}

gcc -Wall -o routeserver.cgi mgtcgi_routeserver.c ${COMMON_STR}
        
gcc -Wall -o configures.cgi mgtcgi_configure.c mgtcgi_netdetection.c mgtcgi_xmlparser.c ${COMMON_STR}

gcc -Wall -o route-configure.cgi mgtcgi_route_configure.c mgtcgi_check.c mgtcgi_common.c mgtcgi_filed_check.c  -L. -lcgic -lxmlwrapper4c

gcc -Wall -o loadrouteconfig loadroute_xmlconf.c mgtcgi_check.c mgtcgi_route_common.c -lxmlwrapper4c

gcc -Wall -o l7-protoctl l7_protoctl.c -lxmlwrapper4c

gcc -Wall -o servconf-ctl servconf-ctl.c -lxmlwrapper4c

gcc -Wall -o loadtiming loadtiming.c  -lxmlwrapper4c

#gcc -Wall -o defroutectl default_route_ctl.c  -lxmlwrapper4c

#gcc -Wall -o getifaceconf get_iface_conf.c  -lxmlwrapper4c

#gcc -Wall -o clearconfig clear_config.c mgtcgi_check.c mgtcgi_route_common.c -lxmlwrapper4c

gcc -Wall -o servconf-ctl servconf-ctl.c -lxmlwrapper4c

gcc -Wall -o portscan.cgi mgtcgi_portscan.c ${COMMON_STR}

gcc -Wall -o servinfo.cgi mgtcgi_servinfo.c ${COMMON_STR}

gcc -Wall -o wifilogin.cgi wifilogin.c ${COMMON_STR}

