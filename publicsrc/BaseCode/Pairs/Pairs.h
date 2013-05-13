#pragma once
#include <windows.h>
#define _PAIRS
namespace PAIRS
{
	CHAR* GetKeyValueStr(CHAR *in_src_str,CHAR *keyname,CHAR *out_value);//返回值为这个key的结尾,"key:value,"结尾的那个key也要这么写必须加,号
	CHAR* split(CHAR *in,CHAR splitFilter,int &count);
};