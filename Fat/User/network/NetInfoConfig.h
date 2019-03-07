#ifndef _NETINFOCONFIG_H
#define _NETINFOCONFIG_H

#include <stdint.h>

extern const uint8_t ucIPAddress[ 4 ] ;
extern const uint8_t ucNetMask[ 4 ] ;
extern const uint8_t ucGatewayAddress[ 4 ] ;
extern const uint8_t ucDNSServerAddress[ 4 ];
extern const uint8_t ucMACAddress[ 6 ] ;

void IP_init( void );
long vSendPing( const int8_t *pcIPAddress );

#endif