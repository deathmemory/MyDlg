#pragma once
#define _STRING_EX
#include <windows.h>
#include <shlwapi.h>
namespace STRING_EX
{

	VOID FreeString(CHAR*pStr);
	CHAR *WideCharToPChar(WCHAR* in_text);
	CHAR *IntStringToString(CHAR *in,DWORD lenstr,CHAR * split);
	CHAR *StringToIntString(CHAR *in,DWORD lenstr,CHAR * split);
	CHAR *MakeRetValue(CHAR *psrc,DWORD lenstr);
	BYTE *IntStringToBuffer(CHAR *in,CHAR * split);
	WCHAR *PCharToWideChar(CHAR* in_text);
	CHAR *WideCharToUtf8(WCHAR* in_text);
	CHAR* PCharToUtf8(CHAR* in);
	BOOL HexStr2Hex(CHAR* InStr, BYTE* OutStr);
	BOOL HexStr2Hex(WCHAR* InStr, BYTE* OutStr);
	CHAR *PUTF8ToChar(CHAR *szInfo);
    
}

