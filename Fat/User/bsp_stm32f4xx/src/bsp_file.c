#include "bsp_file.h"

/* Standard includes. */
#include <stdio.h>
#include <stdarg.h>

/* FreeRTOS+FAT  includes. */
#include "ff_headers.h"
#include "ff_sys.h"
#include "ff_stdio.h"
#include "CHOOSEFILE.h"
#include "GUI.h"

/* Private typedef -----------------------------------------------------------*/
static struct {
  U32 Mask;
  char c;
} _aAttrib[] = {
  { FF_FAT_ATTR_READONLY, 'R' },
  { FF_FAT_ATTR_HIDDEN, 	'H' },
  { FF_FAT_ATTR_SYSTEM, 	'S' },
  { FF_FAT_ATTR_DIR, 			'D' },
  { FF_FAT_ATTR_ARCHIVE, 	'A' },
};
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static char         acAttrib[10];
static char         acExt[4];
/* Private function prototypes -----------------------------------------------*/
void DIRCommand( const char *pcDirectoryToScan )
{
FF_FindData_t *pxFindStruct;
const char  *pcAttrib,
            *pcWritableFile = "writable file",
            *pcReadOnlyFile = "read only file",
            *pcDirectory = "directory";

    /* FF_FindData_t can be large, so it is best to allocate the structure
    dynamically, rather than declare it as a stack variable. */
    pxFindStruct = ( FF_FindData_t * ) pvPortMalloc( sizeof( FF_FindData_t ) );

    /* FF_FindData_t must be cleared to 0. */
    memset( pxFindStruct, 0x00, sizeof( FF_FindData_t ) );

    /* The first parameter to ff_findfist() is the directory being searched.  Do
    not add wildcards to the end of the directory name. */
    if( ff_findfirst( pcDirectoryToScan, pxFindStruct ) == 0 )
    {
        do
        {
            /* Point pcAttrib to a string that describes the file. */
            if( ( pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR ) != 0 )
            {
                pcAttrib = pcDirectory;
            }
            else if( pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY )
            {
                pcAttrib = pcReadOnlyFile;
            }                        
            else
            {
                pcAttrib = pcWritableFile;
            }
						//char s[100];
            /* Print the files name, size, and attribute string. */
            printf("%s [%s] [size=%d]\n", pxFindStruct->pcFileName,
                                                  pcAttrib,
                                                  pxFindStruct->ulFileSize  );
				//		GUI_DispStringAt(s,100,100);
				
        } while( ff_findnext( pxFindStruct ) == 0 );
    }

    /* Free the allocated FF_FindData_t structure. */
    vPortFree( pxFindStruct );
}

void FindPic( const char *pcDirectoryToScan )
{
FF_FindData_t *pxFindStruct;
const char  *pcAttrib,
            *pcWritableFile = "writable file",
            *pcReadOnlyFile = "read only file",
            *pcDirectory = "directory";

    /* FF_FindData_t can be large, so it is best to allocate the structure
    dynamically, rather than declare it as a stack variable. */
    pxFindStruct = ( FF_FindData_t * ) pvPortMalloc( sizeof( FF_FindData_t ) );

    /* FF_FindData_t must be cleared to 0. */
    memset( pxFindStruct, 0x00, sizeof( FF_FindData_t ) );

    /* The first parameter to ff_findfist() is the directory being searched.  Do
    not add wildcards to the end of the directory name. */
    if( ff_findfirst( pcDirectoryToScan, pxFindStruct ) == 0 )
    {
        do
        {
            /* Point pcAttrib to a string that describes the file. */
            if( ( pxFindStruct->ucAttributes & FF_FAT_ATTR_DIR ) != 0 )
            {
                pcAttrib = pcDirectory;
            }
            else if( pxFindStruct->ucAttributes & FF_FAT_ATTR_READONLY )
            {
                pcAttrib = pcReadOnlyFile;
            }                        
            else
            {
                pcAttrib = pcWritableFile;
            }
						//char s[100];
            /* Print the files name, size, and attribute string. */
            printf("%s [%s] [size=%d]\n", pxFindStruct->pcFileName,
                                                  pcAttrib,
                                                  pxFindStruct->ulFileSize  );
				//		GUI_DispStringAt(s,100,100);
				
        } while( ff_findnext( pxFindStruct ) == 0 );
    }

    /* Free the allocated FF_FindData_t structure. */
    vPortFree( pxFindStruct );
} 




/**
  * @brief  Return file extension and removed from file name.
  * @param  pFile: pointer to the file name.
  * @param  pExt:  pointer to the file extension
  * @retval None
  */
static void GetExt(char * pFile, char * pExt) 
{
  int Len;
  int i;
  int j;
  
  /* Search beginning of extension */
  Len = strlen(pFile);
  for (i = Len; i > 0; i--) {
    if (*(pFile + i) == '.') {
      *(pFile + i) = '\0';     /* Cut extension from filename */
      break;
    }
  }
  
  /* Copy extension */
  j = 0;
  while (*(pFile + ++i) != '\0') {
    *(pExt + j) = *(pFile + i);
    j++;
  }
  *(pExt + j) = '\0';          /* Set end of string */
}
/**
  * @brief  This function is responsible to pass information about the requested file
  * @param  pInfo: Pointer to structure which contains all details of the requested file.
  * @retval None
  */
int k_GetData(CHOOSEFILE_INFO * pInfo)
{
	char                c;
  int                 i;
  char               tmp[CHOOSEFILE_MAXLEN];  

	FF_Error_t res = FF_ERR_NONE;
  char *fn;
  FF_FindData_t fno;
  
  switch (pInfo->Cmd) 
  {
  case CHOOSEFILE_FINDFIRST:
//    f_closedir(&dir); 
    
    /* reformat path */
    memset(tmp, 0, CHOOSEFILE_MAXLEN);
		memset(&fno, 0, sizeof(FF_FindData_t));
    strcpy(tmp, pInfo->pRoot);
    
    for(i= CHOOSEFILE_MAXLEN; i > 0 ; i--)
    {
      if(tmp[i] == '/')
      {
        tmp[i] = 0 ;
        break;
      }
    }
    
    res = ff_chdir(tmp);
    
    if (res == pdTRUE)
    {
      
      res = ff_findfirst(tmp,&fno);
    }
    break;
    
  case CHOOSEFILE_FINDNEXT:
    res = ff_findnext(&fno);
    break;
  }
  
  if (res == FF_ERR_NONE)
  {
    fn = (char *)fno.pcFileName;
    
    while (((fno.ucAttributes & FF_FAT_ATTR_DIR) == 0) && (res == FF_ERR_NONE))
    {
 
      if((strstr(pInfo->pMask, ".img")))
      {
        if((strstr(fn, ".bmp")) || (strstr(fn, ".jpg")) || (strstr(fn, ".BMP")) || (strstr(fn, ".JPG")))
        {
          break;
        }
        else
        {
//          res = f_readdir(&dir, &fno);
          
					/* Point pcAttrib to a string that describes the file. */
//					if( fno.ucAttributes & FF_FAT_ATTR_READONLY )
//					{
//							pcAttrib = pcReadOnlyFile;
//					}                        
//					else
//					{
//							pcAttrib = pcWritableFile;
//					}
//					
//					
//          if (res != FF_ERR_NONE || fno.fname[0] == 0)
//          {
//            f_closedir(&dir); 
//            return 1;
//          }
//          else
//          
//            fn = fno.pcFileName;
//          }
        }
        
      }     
      else if(strstr(fn, pInfo->pMask) == NULL)
      {
        
//        res = f_readdir(&dir, &fno);
//        
//        if (res != FR_OK || fno.fname[0] == 0)
//        {
//          f_closedir(&dir); 
//          return 1;
//        }
//        else
//        {
//          fn = fno.fname;
//        }
      }
      else
      {
        break;
      }
    }   
    
    if(fn[0] == 0)
    {
 //     f_closedir(&dir); 
      return 1;
    } 
    
    pInfo->Flags = ((fno.ucAttributes & FF_FAT_ATTR_DIR) != 0) ? CHOOSEFILE_FLAG_DIRECTORY : 0;
    
    for (i = 0; i < GUI_COUNTOF(_aAttrib); i++)
    {
      if (fno.ucAttributes & _aAttrib[i].Mask)
      {
        c = _aAttrib[i].c;
      }
      else
      {
        c = '-';
      }
      acAttrib[i] = c;
    }
    if((fno.ucAttributes & FF_FAT_ATTR_DIR) != 0)
    {
      acExt[0] = 0;
    }
    else
    {
 //     GetExt(fn, acExt);
    }
    pInfo->pAttrib = acAttrib;
    pInfo->pName = fn;
    pInfo->pExt = acExt;
    pInfo->SizeL = fno.ulFileSize;
    pInfo->SizeH = 0;
    
  }
  return res;
}















