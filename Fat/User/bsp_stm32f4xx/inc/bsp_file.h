#ifndef __BSP_FILE_H_
#define __BSP_FILE_H_
#include "stdint.h"
#include "CHOOSEFILE.h"
//typedef struct 
//{
//	uint8_t 
//	uint8_t *fileName,
//}picFileList;

void DIRCommand( const char *pcDirectoryToScan );
int k_GetData(CHOOSEFILE_INFO * pInfo);
#endif