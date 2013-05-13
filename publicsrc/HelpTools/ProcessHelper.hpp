#pragma once

//////////////////////////////////////////////////////////////////////////
// CProcessHelper

#include <tlhelp32.h> 
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);
static PROCNTQSIP NtQueryInformationProcess = NULL;	// ntdll

typedef LONG	NTSTATUS;
typedef ULONG	PPS_POST_PROCESS_INIT_ROUTINE;
#define ProcessBasicInformation				0

typedef struct _PEB_LDR_DATA {
	BYTE       Reserved1[8];
	PVOID      Reserved2[3];
	LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

typedef struct _RTL_USER_PROCESS_PARAMETERS {
	BYTE           Reserved1[16];
	PVOID          Reserved2[10];
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
	BYTE                          Reserved1[2];
	BYTE                          BeingDebugged;
	BYTE                          Reserved2[1];
	PVOID                         Reserved3[2];
	PPEB_LDR_DATA                 Ldr;
	PRTL_USER_PROCESS_PARAMETERS  ProcessParameters;
	BYTE                          Reserved4[104];
	PVOID                         Reserved5[52];
	PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
	BYTE                          Reserved6[128];
	PVOID                         Reserved7[1];
	ULONG                         SessionId;
} PEB, *PPEB;

typedef struct _PROCESS_BASIC_INFORMATION {
	PVOID Reserved1;
	PPEB PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION;


class CProcessHelper
{
public:
	CProcessHelper(){}
	virtual ~CProcessHelper(){}
public:
	BOOL PromoteLimit()
	{
		HANDLE hToken;
		LUID sedebugnameValue;
		TOKEN_PRIVILEGES tkp;
		if ( ! OpenProcessToken( GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
		{
			//WriteToLog("openprocesstoken error");
			return FALSE;
		}
		if ( ! LookupPrivilegeValue( NULL, SE_DEBUG_NAME, &sedebugnameValue ) )
		{
			CloseHandle( hToken );
			//WriteToLog("lookuprivilegevalue error");
			return FALSE;
		}
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Luid = sedebugnameValue;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		if ( ! AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL ) )
		{
			CloseHandle( hToken );
			return FALSE;
		}
		return TRUE;
		//    MessageBox("提权成功!");
		//提权代码，差不多已成套路了没什么可说的。
		//////////////////////////////////////////////////////////////////////////
	}
	BOOL ScanProcess(CString strProcName)
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		PROCESSENTRY32* processInfo = new PROCESSENTRY32;
		processInfo->dwSize = sizeof(PROCESSENTRY32);
		while(Process32Next(hSnapShot,processInfo)!=FALSE)
		{
			if(strProcName.CompareNoCase(processInfo->szExeFile) == 0)
			{
				delete processInfo;
				return true;
			}
		}
		CloseHandle(hSnapShot);
		delete processInfo;
		return false;
	}

	BOOL KillProcByName(CString strProcName)
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		//现在获得了所有进程的信息。
		//将从hSnapShot中抽取数据到一个PROCESSENTRY32结构中
		//这个结构代表了一个进程，是ToolHelp32 API的一部分。
		//抽取数据靠Process32First()和Process32Next()这两个函数。
		//这里仅用Process32Next()，他的原形是：
		//BOOL WINAPI Process32Next(HANDLE hSnapshot,LPPROCESSENTRY32 lppe);
		PROCESSENTRY32* processInfo = new PROCESSENTRY32;
		// 必须设置PROCESSENTRY32的dwSize成员的值 ;
		processInfo->dwSize = sizeof(PROCESSENTRY32);
		//这里将快照句柄和PROCESSENTRY32结构传给Process32Next()。
		//执行之后，PROCESSENTRY32 结构将获得进程的信息。循环遍历，直到函数返回FALSE。
		//printf("****************开始列举进程****************\n");
		while(Process32Next(hSnapShot,processInfo)!=FALSE)
		{
			// 		char szBuf[MAX_PATH];
			// 		wcstombs(szBuf, processInfo->szExeFile, MAX_PATH);
			if(strProcName.CompareNoCase(processInfo->szExeFile) == 0)
			{
				int ID = processInfo->th32ProcessID;
				HANDLE hProcess;
				// 现在我们用函数 TerminateProcess()终止进程：
				// 这里我们用PROCESS_ALL_ACCESS
				hProcess = OpenProcess(PROCESS_ALL_ACCESS,TRUE,ID);
				if(hProcess == NULL)
				{
					//qDebug("打开进程失败 !\n");
					delete processInfo;
					return false;
				}
				TerminateProcess(hProcess,0);
				CloseHandle(hProcess);
			}
		}
		CloseHandle(hSnapShot);
		delete processInfo;
		return true;
	}

	void GetProcessCMDLine(__in LPTSTR lpProcName, __out LPTSTR lpstrBuff, __in int nSize)
	{
		PROCESS_BASIC_INFORMATION   pbi = {0};   
		PEB                         Peb = {0};   
		RTL_USER_PROCESS_PARAMETERS ProcParam = {0};   
		DWORD                       dwDummy = 0;   
		DWORD                       dwSize = 0;   
		LPVOID                      lpAddress = NULL;   
		BOOL                        bRet = FALSE;  

		HANDLE hProcess = GetProcessHandle(lpProcName);
		if (hProcess)
		{
			NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress( GetModuleHandle(_T("ntdll")), "NtQueryInformationProcess" ); 
			if (NtQueryInformationProcess)
			{
				LONG lnStatus = NtQueryInformationProcess(hProcess, ProcessBasicInformation, 
					(PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
				if (lnStatus)   
					goto cleanup;   

				if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress,   
					&Peb, sizeof(PEB), &dwDummy ) )   
					goto cleanup;   

				if (!ReadProcessMemory(hProcess, Peb.ProcessParameters,
					&ProcParam, sizeof(RTL_USER_PROCESS_PARAMETERS), &dwDummy) )   
					goto cleanup;   

				lpAddress = ProcParam.CommandLine.Buffer;   
				dwSize = ProcParam.CommandLine.Length;   

				if (nSize<dwSize)   
					goto cleanup;   

				if ( !ReadProcessMemory(hProcess, lpAddress, lpstrBuff, dwSize, &dwDummy ) )   
					goto cleanup;   
				bRet = TRUE;   
			}
cleanup:   
			CloseHandle (hProcess);   
		}
	}

	HANDLE GetProcessHandle(__in LPTSTR lpProcName)
	{
		DWORD aProcessIds[2048] = {0};
		DWORD dwProcessCount = 0;
		if (EnumProcesses(aProcessIds, 2048, &dwProcessCount))
		{
			for (DWORD i = 0; i < dwProcessCount; ++i)
			{
				HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, aProcessIds[i]);
				if (hProcess)
				{
					TCHAR cExeName[MAX_PATH] = {0};
					if (GetModuleBaseName(hProcess, nullptr, cExeName, MAX_PATH))
					{
						if (_tcsicmp(cExeName, lpProcName) == 0)
						{
							return hProcess;
						}
					}
					CloseHandle(hProcess);
				}
			}
		}
		return NULL;
	}

	BOOL ScanDllInProcess(DWORD dwPID, CString strDllName)
	{
		BOOL bRet = FALSE;
		MODULEENTRY32 pe32;
		// 在使用这个结构之前，先设置它的大小
		pe32.dwSize = sizeof(pe32); 
		// 给进程内所有模块拍一个快照
		//276为某进程的ID
		HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
		if(hProcessSnap == INVALID_HANDLE_VALUE)
		{       
			//建立快照失败
			return bRet;  
		}

		// 遍历进程快照，轮流显示每个进程的信息
		BOOL bMore = Module32First(hProcessSnap, &pe32);
		while(bMore)
		{       
			// 			printf("\n[DLL NAME]\t%s\n",pe32.szModule);
			// 			printf("[DLL PATH]\t%s\n",pe32.szExePath);
			if ( strDllName.CompareNoCase(pe32.szModule) == 0)
			{
				bRet = TRUE;
				break;
			}

			bMore = Module32Next(hProcessSnap, &pe32);
		}
		// 不要忘记清除掉snapshot对象
		CloseHandle(hProcessSnap);
	}

	BOOL ScanDllInProcess(CString strProcName, CString strDllName)
	{
		BOOL bRet = FALSE;
		DWORD dwPID = 0;
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
		PROCESSENTRY32* processInfo = new PROCESSENTRY32;
		processInfo->dwSize = sizeof(PROCESSENTRY32);
		while(Process32Next(hSnapShot,processInfo)!=FALSE)
		{
			if(strProcName.CompareNoCase(processInfo->szExeFile) == 0)
			{
				dwPID = processInfo->th32ProcessID;
				break;
			}
		}
		CloseHandle(hSnapShot);
		delete processInfo;

		if (dwPID)
		{
			bRet = ScanDllInProcess(dwPID, strDllName);
		}
		return bRet;
	}
};