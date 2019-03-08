
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "bsp_lan8720.h"   
#include "lwip/opt.h"

#include "lwip/init.h"
#include "lwip/stats.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/ip.h"
#include "lwip/raw.h"
#include "lwip/udp.h"

#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "netif/etharp.h"
#include "lwip/api.h"
#include "lwip/dhcp.h"
#include "arch/ethernetif.h"
	
void LwIP_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */


