#include "ff_stdio.h"
#include "GUI.h"

#define PIC_BUFFRE_SIZE  (800*4)

typedef struct 
{
	FF_FILE *pcFile;
	uint8_t *_acBuffer;
}IN_DATA;

/**
  * @brief  BMP/GIF/JPEG获取外部图片文件数据的回调函数，作为GUI_xx_DrawEx的回调函数
  * @param  p: 外部调用时输入的指针，用户自定义的数据指针，在这里为结构体指针
  * @param  NumBytesReq:请求的字节数。
  * @param 	ppData：指向到获取到数据的指针。“GetData”函数必须将指针设置到请求数据所在的有效位置。
  * @param  Off ：如果此标记为 1，应将数据指针设置到数据流的起始位置
  * @retval 返回可用的数据字节数
  */
int _GetData_BMP_GIF_JPEG(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off)
{
	IN_DATA *in_data;
	U32 NumBytesRead;
	U32 Offset = 0;
	int 			iReturned;
	in_data = (IN_DATA *)p;
	//
	// Check buffer size
	//
	if (NumBytesReq > (PIC_BUFFRE_SIZE)) {
		NumBytesReq = (PIC_BUFFRE_SIZE);
	}
	
	//
	// Set file pointer to the required position
	//
	if(Off == 1)
		Offset = 0;
	else 
		Offset = Off;
	iReturned = ff_fseek(in_data->pcFile,Offset,FF_SEEK_SET);
	if(iReturned)
	{
		printf("file seek err\n");
	}
	//
	// Read data into buffer
	//
	NumBytesRead = ff_fread(in_data->_acBuffer,1,NumBytesReq,in_data->pcFile);
	//
	// Set data pointer to the beginning of the buffer
	//
	*ppData =in_data->_acBuffer;
	//
	// Return number of available bytes
	//
	printf("file read num : %ld\n",NumBytesRead);
	return NumBytesRead;
}
/**
  * @brief  PNG/BITMAPS获取外部图片文件数据的回调函数，作为GUI_xx_DrawEx的回调函数
  * @param  p: 外部调用时输入的指针，一般使用文件指针.
  * @param  NumBytesReq:请求的字节数。
  * @param 	ppData：获取到的数据要填充的指针。必须使用 “GetData”函数填充指针指向的位置。
  * @param  Off ：如果此标记为 1，应将数据指针设置到数据流的起始位置
  * @retval 返回可用的数据字节数
  */
int _GetData_PNG_BITMAPS(void * p, const U8 **ppData, unsigned int NumBytesReq, U32 Off)
{
	
	FF_FILE *	pcFile;
	U32 			NumBytesRead;
	U32 			Offset = 0; 
	U8  	  * pData;
	int 			iReturned;
	
	pcFile=(FF_FILE *)p;
	pData  = (U8 *)*ppData;
	
	/*
	* 根据Off参数设置位置
	*/	
	if(Off == 1)
		Offset = 0;
	else 
		Offset = Off;
	
	iReturned = ff_fseek(pcFile,Offset,FF_SEEK_SET);
	if(iReturned)
	{
	//	iReturned = ff_errno();
		printf("seek err \n");
	}
//	printf("seek ok \n");
	/*
	* 读取数据到指针的位置
	*/
	NumBytesRead = ff_fread(pData,1,NumBytesReq,pcFile);
	/*
	* 返回读取到的有效字节数
	*/
//	printf("read num:%ld \n",NumBytesRead);
	return NumBytesRead;
}
/**
  * @brief  显示文件系统中的BMP图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _ShowBMPEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) 
{	 
	FF_Error_t	xError = FF_ERR_NONE;   
	int 			iReturned; 

	IN_DATA in_data;
	
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
	//把空间句柄转成指针
	in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FF_FILE));

	//把空间句柄转成指针
	in_data.pcFile = GUI_ALLOC_h2p(hMem2);
	
    /* 打开文件 */		
	in_data.pcFile = ff_fopen( sFilename, "r");
  
	if (in_data.pcFile != NULL)
	{
		printf("file open ok\n");
	
		iReturned = GUI_BMP_DrawEx(_GetData_BMP_GIF_JPEG, &in_data, usPOSX, usPOSY);

		ff_fclose(in_data.pcFile);
		printf("iReturned is %d ",iReturned);
	}
	else
	{
		/* open err*/
	}
	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);

}
/**
  * @brief  显示文件系统中的JPEG图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _ShowJPEGEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY)  
{	 
	int 			iReturned; 

	IN_DATA in_data;
	
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
	//把空间句柄转成指针
	in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FF_FILE));

	//把空间句柄转成指针
	in_data.pcFile = GUI_ALLOC_h2p(hMem2);

	
	/* 打开文件 */		
	in_data.pcFile = ff_fopen( sFilename, "r");
	if (in_data.pcFile != NULL)
	{
	//	printf(" open ok\n");
		iReturned = GUI_JPEG_DrawEx(_GetData_BMP_GIF_JPEG, &in_data, usPOSX, usPOSY);
		printf("iReturned is %d ",iReturned);
		ff_fclose(in_data.pcFile);
	}
	else
	{
		/* err */
	}
	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);

}
 /**
  * @brief  显示文件系统中的GIF图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @param 	ctime：GIF图片循环显示多少次，
                   若输入ctime为0,则一直循环显示GIF图片，本函数内部会死循环
  * @retval 无
  */
void _ShowGIFEx(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY,uint32_t ctime)  
{	  
	FF_Error_t	xError = FF_ERR_NONE; 	
 	GUI_GIF_INFO 				GIFInfo;
	GUI_GIF_IMAGE_INFO  GIF_ImageInfo;
	uint16_t		i=0;
	uint32_t    t0,t1;
	uint8_t 		forver =  0;
		
	IN_DATA in_data;
	
	GUI_HMEM hMem,hMem2;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(PIC_BUFFRE_SIZE);
	//把空间句柄转成指针
	in_data._acBuffer = GUI_ALLOC_h2p(hMem);
	
	//从emwin内存中分配空间
	hMem2 = GUI_ALLOC_AllocZero(sizeof(FF_FILE));

	//把空间句柄转成指针
	in_data.pcFile = GUI_ALLOC_h2p(hMem2);

	
	//若输入的ctime == 0，GIF图片一直循环显示
	if(ctime == 0) forver =1;	

	/* 打开文件 */		
	in_data.pcFile = ff_fopen( sFilename, "r");
	if (in_data.pcFile != NULL)
	{
		//获取GIF图片信息
		GUI_GIF_GetInfoEx(_GetData_BMP_GIF_JPEG,&in_data,&GIFInfo);

		while(ctime-- || forver)
		{
			i=0;
			//循环显示所有子图片
			while(i<GIFInfo.NumImages)
			{
				GUI_GIF_GetImageInfoEx(_GetData_BMP_GIF_JPEG,&in_data,&GIF_ImageInfo,i);
			
				//emwin要求，当GIF_ImageInfo.Delay ==0 时，要延时100ms
				if(GIF_ImageInfo.Delay == 0)
					GUI_Delay(100);
				
				t0 = GUI_GetTime();
				GUI_GIF_DrawSubEx(_GetData_BMP_GIF_JPEG,
														&in_data, 
														usPOSX, 
														usPOSY,
														i++);
				//计算解码本身消耗的时间
				t1 = GUI_GetTime()-t0;
				
				//GIF_ImageInfo.Delay的单位为10ms，所以乘以10
				if(t1 < GIF_ImageInfo.Delay*10)
				{
					GUI_Delay(GIF_ImageInfo.Delay*10-t1);
				}			
			}	
		}

		ff_fclose(in_data.pcFile);
	}
	else
	{
		/* open err*/
	}
	
	//释放申请的空间
	GUI_ALLOC_Free(hMem);
		//释放申请的空间
	GUI_ALLOC_Free(hMem2);

}

/**
  * @brief  显示文件系统中的PNG图片
  * @param  sFilename: 图片的完整路径
  * @param  usPOSX:图片的显示位置X
  * @param 	usPOSY：图片的显示位置Y
  * @retval 无
  */
void _DrawPNG_file(const char * sFilename, uint16_t usPOSX, uint16_t usPOSY) 
{  
	FF_Error_t	xError = FF_ERR_NONE;   

	FF_FILE *pcFile;
	
	GUI_HMEM hMem;	
  
	//从emwin内存中分配空间
	hMem = GUI_ALLOC_AllocZero(sizeof(FF_FILE));

	//把空间句柄转成指针
	pcFile = GUI_ALLOC_h2p(hMem);

	
	/* 打开文件 */		
	pcFile = ff_fopen( sFilename, "r");
	if (pcFile != NULL)
	{
	//	printf("open ok \n");
		GUI_PNG_DrawEx(_GetData_PNG_BITMAPS, pcFile, usPOSX, usPOSY); 
			
		ff_fclose(pcFile);
	//	printf("close ok \n");		
	}  
	else
	{
		/* open err*/
		printf("open err \n");
	}
	//释放申请的空间
	GUI_ALLOC_Free(hMem);
}
/*********************************************END OF FILE**********************/
int _BMP_GIF_JPEG_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {
	static char _acBuffer[2400] ;
	FF_FILE * phFile;
	char *fn;
	U32 NumBytesRead;
	U32 Offset = 0;
	int iReturned;
	U8 *pData = (U8 *)*ppData;
	fn = (char *)p;
	//
	// Check buffer size
	//
	if(NumBytes> sizeof(_acBuffer))
		NumBytes = sizeof(_acBuffer);
	phFile = ff_fopen( fn, "r");
	if(phFile!= NULL)
	{
		//
		// Set file pointer to the required position
		//
		if(Off == 1) Offset = 0;
		else  Offset = Off;
		iReturned = ff_fseek(phFile,Offset,FF_SEEK_SET);
		if(iReturned)
		{
			printf("seek err \n");
		}
		//
		// Read data into buffer
		//
		NumBytesRead = ff_fread(_acBuffer,1,NumBytes,phFile);
		//
		// Set data pointer to the beginning of the buffer
		//
		*ppData = _acBuffer;
		//
		// Return number of available bytes
		//
		ff_fclose(phFile);
		return NumBytesRead;
	}
	else
	{
		printf("open  %s err \n",fn);
		return 0;
	}
}

int _PNG_BMPstream_GetData(void * p, const U8 ** ppData, unsigned NumBytes, U32 Off) {
	FF_FILE * phFile;
	char *fn;
	U32 NumBytesRead;
	U32 Offset = 0;
	fn = (char *)p;
	U8  	  * pData;
	int iReturned;
	
	pData  = (U8 *)*ppData;
	//
	// Check buffer size
	//
	phFile = ff_fopen( fn, "r");
	if(phFile!= NULL)
	{
		//
		// Set file pointer to the required position
		//
		if(Off == 0xFFFFFFFF) Offset = 0;
		else  Offset = Off;
		iReturned = ff_fseek(phFile,Offset,FF_SEEK_SET);
		if(iReturned)
		{
			printf("seek err \n");
		}
		printf("seedk %ld",Offset);
		//
		// Read data into buffer
		//
		NumBytesRead = ff_fread(pData,1,NumBytes,phFile);

		//
		// Return number of available bytes
		//
		ff_fclose(phFile);
		return NumBytesRead;
	}
	else
	{
		printf("open err \n");
		return 0;
	}
}
	












