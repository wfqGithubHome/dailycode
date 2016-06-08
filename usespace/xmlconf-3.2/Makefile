.KEEP_STATE:


all:    lib

#CXX     = /usr/local/arm-xscale/bin/armv5b-softfloat-linux-g++ -mbig-endian
#GCC	= /usr/local/arm-xscale/bin/armv5b-softfloat-linux-gcc -mbig-endian
#CXXFLAGS = -Wall -Wno-deprecated -I ../opt-3.19/src/
#LIB     = -L ../opt-3.19/src/ -l opt 
CXX     = g++
GCC	= gcc
CXXFLAGS = -g -Wall -Wno-deprecated 
CXXLIBFLAGS = -fPIC -shared -pthread
LIB     = -L /usr/local/lib/ -l opt 

main_object = xmlconf.o

tinyxml_object = tinyxml/tinyxml.o tinyxml/tinystr.o tinyxml/tinyxmlerror.o tinyxml/tinyxmlparser.o

#xml_object = xml/reader.o xml/writer.o xml/parser.o
xml_object = xml/reader.o xml/writer.o xml/parser.o xml/rwxml.o xml/route_parser.o xml/guide_parser.o

core_object += core/system.o core/management.o core/ddns.o core/auth.o core/interface.o core/interfacemgr.o 
core_object += core/session.o core/sessionmgr.o core/logger.o core/node.o core/nodemgr.o core/netaddr.o 
core_object += core/netaddrmgr.o core/vlan.o core/vlanmgr.o core/schedule.o core/schedulemgr.o core/protocol.o 
core_object += core/protocolmgr.o core/traffic_channel.o core/traffic_util.o core/traffic_id.o core/trafficmgr.o 
core_object += core/firewall.o core/firewallmgr.o core/globalip.o core/globalipmgr.o core/portmirror.o core/portmirrormgr.o 
core_object += core/macbind.o core/macbindmgr.o core/macauth.o core/macauthmgr.o core/quota.o core/quotamgr.o 
core_object += core/domainnode.o core/domainmgr.o core/dnspolicy.o core/dnspolicymgr.o 
core_object += core/extensionnode.o core/extensionmgr.o core/httpservernode.o core/httpservermgr.o 
core_object += core/urlformatnode.o core/urlformatmgr.o core/httpdirpolicynode.o core/httpdirpolicymgr.o
core_object += core/xmllib_schedulemgr.o core/layer7prototype.o core/layer7prototypemgr.o 
core_object += core/portscan.o core/portscanmgr.o
core_object += core/servinfo.o core/servinfomgr.o
core_object += core/servconf.o core/servconfmgr.o core/networkdetection.o
core_object += core/timing.o core/timingmgr.o
core_object += core/wifi.o core/wifimgr.o
core_object += core/firstgamechannel.o core/firstgamechannelmgr.o
core_object += core/routeserver.o
core_object += core/firewall_policy.o core/firewall_policymgr.o
core_object += core/channel.o core/wan_channel.o core/wan_channelmgr.o

core_object += core/route_dns.o core/route_lan.o core/route_lanmgr.o core/route_wan.o core/route_wanmgr.o
core_object += core/route_pppd_authenticate.o core/route_adsl.o core/route_adslmgr.o 
core_object += core/route_vpn.o core/route_vpnmgr.o core/route_routing.o core/route_routingmgr.o
core_object += core/route_table.o core/route_tablemgr.o core/route_rule.o core/route_rulemgr.o 
core_object += core/route_dnat.o core/route_dnatmgr.o core/route_snat.o core/route_snatmgr.o
core_object += core/route_loadbalancing.o core/route_loadbalancingmgr.o
core_object += core/route_macbind.o core/route_macbindmgr.o
core_object += core/route_vlan.o core/route_vlanmgr.o
core_object += core/route_interfacelink.o core/route_interfacelinkmgr.o
core_object += core/route_macvlan.o core/route_macvlanmgr.o
core_object += core/route_tables.o core/route_tablesmgr.o

core_object += core/guide/guide_adsl.o core/guide/guide_adslmgr.o
core_object += core/guide/guide_appbind.o core/guide/guide_appbindmgr.o
core_object += core/guide/guide_dnat.o
core_object += core/guide/guide_lan.o core/guide/guide_lanmgr.o
core_object += core/guide/guide_server.o core/guide/guide_servermgr.o
core_object += core/guide/guide_tc.o core/guide/guide_tcmgr.o
core_object += core/guide/guide_wan.o core/guide/guide_wanmgr.o
core_object += core/guide/guide_zone.o core/guide/guide_zonemgr.o
core_object += core/route_dhcpd.o core/route_dhcpdmgr.o
core_object += core/route_dhcpdhost.o core/route_dhcpdhostmgr.o
core_object += core/route_pptpd.o core/route_pptpdmgr.o
core_object += core/route_pptpd_account.o core/route_pptpd_accountmgr.o

config_object += config/config.o config/route_config.o config/guide_config.o

base_object = base/log/filelog.o base/log/syslog.o base/log/consolelog.o base/log.o base/logwraper.o base/exception.o 

debug_object = debug/debug_new.o

object = $(tinyxml_object) $(base_object) $(config_object) $(core_object)  $(xml_object) $(main_object)

xmllibobject += libxmlwrapper4c/xmlwrapper4c.o libxmlwrapper4c/route_xmlwrapper4c.o libxmlwrapper4c/guide_xmlwrapper4c.o

#xmlconf: $(object)
#	$(CXX) $(CXXFLAGS) $(object) $(LIB)  -o xmlconf_debug

libobject = $(tinyxml_object) $(base_object) $(core_object) $(xml_object) $(config_object) $(xmllibobject)
lib: $(libobject)
	$(CXX) $(CXXFLAGS) $(CXXLIBFLAGS) $(libobject) $(LIB)   -o  libxmlwrapper4c.so
	cp --force libxmlwrapper4c.so  /lib/

%.o:    %.cc
	$(CXX) $(CXXFLAGS)  -c $< -o $@

clean:
	${RM} *.o config/*.o base/*.o base/log/*.o debug/*.o core/*.o \
	tinyxml/*.o xml/*.o libxmlwrapper4c/*.o libxmlwrapper4c/*.so  *.so
