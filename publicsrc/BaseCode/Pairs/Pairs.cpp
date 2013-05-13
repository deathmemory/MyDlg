#include "Pairs.h"
namespace PAIRS
{
	CHAR* GetKeyValueStr(CHAR *in_src_str,CHAR *keyname,CHAR *out_value)//返回值为这个key的结尾,"key:value,"结尾的那个key也要这么写必须加,号
	{
		
		if((!in_src_str) || (!keyname) || (!out_value))
		{
			return 0;
		}
		
		CHAR *valuePtr=0,*valueEndFlag=0;
		CHAR *reKey = (CHAR*)malloc(lstrlenA(keyname)+4);
		INT cpyLen=0;
		*out_value=0;

		wsprintfA(reKey,"%s:",keyname);
		valuePtr = StrStrIA(in_src_str,reKey);
		if((!valuePtr) || ((valuePtr[-1]!=',') && (valuePtr!=in_src_str)))
		{
			free(reKey);
			
			return 0;
		}
		valuePtr = (CHAR*)(valuePtr+lstrlenA(reKey));
		valueEndFlag = StrStrIA(valuePtr,",");
		cpyLen = (INT)(valueEndFlag-valuePtr)+1;
		lstrcpynA(out_value,valuePtr,cpyLen);
		
		free(reKey);
		return (CHAR*)(valuePtr+cpyLen);
	}
	CHAR* split(CHAR *in,CHAR splitFilter,int &count)
	{
		CHAR *dest = (CHAR*)(GlobalAlloc(GPTR,lstrlenA(in)+1));
		CHAR *ptr = in;
		while(ptr)
		{
			ptr = StrChrA(ptr,splitFilter);
			if(ptr)*ptr=0;
			count++;
		}
		return dest;
	} 
};