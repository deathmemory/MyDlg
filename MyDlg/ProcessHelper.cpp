#include "StdAfx.h"
#include "ProcessHelper.h"
#include <Psapi.h>
#pragma comment(lib, "psapi.lib")

typedef LONG (WINAPI *PROCNTQSIP)(HANDLE, UINT, PVOID, ULONG, PULONG);
PROCNTQSIP NtQueryInformationProcess = NULL;	// ntdll

CProcessHelper::CProcessHelper(void)
{
}


CProcessHelper::~CProcessHelper(void)
{
}
void CProcessHelper::GetProcessCMDLine(__in LPTSTR lpProcName, __out LPTSTR lpstrBuff, __in int nSize)
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

HANDLE CProcessHelper::GetProcessHandle(__in LPTSTR lpProcName)
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