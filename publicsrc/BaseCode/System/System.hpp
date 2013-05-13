#pragma once
#define _SYSTEM_INTERFACE
#ifdef _WINDOWS
#include "stdafx.h"
#endif
#include <windows.h>
#include <Wininet.h>
#include <Shlwapi.h>
#pragma comment(lib,"shlwapi.lib")
#include "../stdlib/cRandom.h"
#ifndef _WINDOWS
#define LPCSTR CHAR *
#define CHAR char
#endif
/*
#ifdef _DEBUG
#define NEW(var,s) s *var = (s*)GlobalAlloc(GPTR,sizeof(s));\
__asm{JMP $cbk}\
__asm{$cCt:}\
s::s();\
__asm {$cbk:}\
__asm {MOV ECX,var}\
__asm {MOV EAX,$cCt}\
__asm {LEA EAX,[EAX+7]}\
__asm {LEA EBX,[EAX-1]}\
__asm {MOV EAX,[EAX]}\
__asm {ADD EBX,EAX}\
__asm {ADD EBX,5}\
__asm {CALL EBX}

#define DEL(var,s)if(var){_asm{JMP $$cbk}\
__asm{$$cCt:}\
s::s();\
__asm {$$cbk:}\
__asm {MOV ECX,var}\
__asm {MOV EAX,$$cCt}\
__asm {LEA EAX,[EAX+18]}\
__asm {LEA EBX,[EAX-1]}\
__asm {MOV EAX,[EAX]}\
__asm {ADD EBX,EAX}\
__asm {ADD EBX,5}\
__asm {CALL EBX}\
GlobalFree(var);}
#else
#define NEW(var,s) s *var = (s*)GlobalAlloc(GPTR,sizeof(s));\
__asm{JMP $cbk}\
__asm{$cCt:}\
s::s();\
__asm {$cbk:}\
__asm {MOV EAX,$cCt}\
__asm {LEA EAX,[EAX+4]}\
__asm {LEA EBX,[EAX-1]}\
__asm {MOV EAX,[EAX]}\
__asm {ADD EBX,EAX}\
__asm {ADD EBX,5}\
__asm {MOV EAX,var}\
__asm {CALL EBX}

#define DEL(var,s)if(var){_asm{JMP $$cbk}\
__asm{$$cCt:}\
s::s();\
__asm {$$cbk:}\
__asm {MOV EAX,$$cCt}\
__asm {LEA EAX,[EAX+9]}\
__asm {LEA EBX,[EAX-1]}\
__asm {MOV EAX,[EAX]}\
__asm {ADD EBX,EAX}\
__asm {ADD EBX,5}\
__asm {MOV EAX,var}\
__asm {CALL EBX}\
GlobalFree(var);}
#endif
*/

namespace SYSTEM_INTERFACE
{
 	enum MAKE_TYPE
	{
		SCRIPT_BEGIN = 0,
		SCRIPT_UPDATE = 1,
		SCRIPT_COPY = 2,
		SCRIPT_DELETE = 3,
		SCRIPT_RUN = 4,
		SCRIPT_SEARCH_AND_DEL = 5,
		SCRIPT_END = 6
	};
	CHAR *GetIEChcheFilePath(CHAR *lpszUrl);
	CHAR *GetIEChcheFileShortPath(CHAR *lpszUrl);
	//BOOL WINAPI _GetUrlCacheEntryInfoA(  LPCSTR lpszUrlName,LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo,  LPDWORD lpcbCacheEntryInfo);
	//BOOL WINAPI _VirtualProtectEx(  HANDLE hProcess,  LPVOID lpAddress,  SIZE_T dwSize,  DWORD flNewProtect, PDWORD lpflOldProtect);
	//BOOL WINAPI _WriteProcessMemory(      HANDLE hProcess,      LPVOID lpBaseAddress, LPCVOID lpBuffer,      SIZE_T nSize, SIZE_T * lpNumberOfBytesWritten);
	//BOOL WINAPI _VirtualProtect(  LPVOID lpAddress,  SIZE_T dwSize,  DWORD flNewProtect, PDWORD lpflOldProtect);
	//SIZE_T WINAPI _VirtualQuery( LPCVOID lpAddress,PMEMORY_BASIC_INFORMATION lpBuffer,     SIZE_T dwLength);
	//HANDLE WINAPI _OpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
	//BOOL WINAPI _ReadProcessMemory(      HANDLE hProcess,      LPCVOID lpBaseAddress,LPVOID lpBuffer,      SIZE_T nSize, SIZE_T * lpNumberOfBytesRead);
	CHAR* MakeUpdateScript(MAKE_TYPE mtype,CHAR* path1,CHAR *path2);
	CHAR* GetCurrentDirA();
	VOID FreeScript();
	VOID PathPatch(CHAR *Path);//Â·¾¶½áÎ²²¹ Ð±¸Ü
	DWORD ClearUrlCacheEntry(CHAR *url);
	WCHAR* GetCurrentDirW();
	CHAR *GetDeviceDescList(CHAR *DeviceName);
	BOOL ScanPowerSpeed();
	HWND SearchWindow(CHAR *pLikeTitle);
}
