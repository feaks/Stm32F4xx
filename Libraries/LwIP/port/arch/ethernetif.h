#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* MAC ADDRESS*/
#define MAC_ADDR0   0x40
#define MAC_ADDR1   0x8D
#define MAC_ADDR2   0x5C
#define MAC_ADDR3   0xDB
#define MAC_ADDR4   0xB2
#define MAC_ADDR5   0x18
 
/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   28
#define IP_ADDR3   10
   
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   28
#define GW_ADDR3   1  



err_t ethernetif_init(struct netif *netif);



#endif 
