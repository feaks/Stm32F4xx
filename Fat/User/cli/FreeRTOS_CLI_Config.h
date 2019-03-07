#ifndef __FREERTOS_CLI_CONFIG_H
#define __FREERTOS_CLI_CONFIG_H



extern void vRegisterFileSystemCLICommands( void );
extern void vUARTCommandConsoleStart( uint16_t usStackSize, UBaseType_t uxPriority );


#endif