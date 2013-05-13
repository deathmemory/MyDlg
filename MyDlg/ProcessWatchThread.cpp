// ProcessWatchThread.cpp : implementation file
//

#include "stdafx.h"
#include "ProcessWatchThread.h"
#include "Psapi.h"
#include "../Config/Config.h"
#include "../publicsrc/publicdef.h"
// #include "Messages.h"

// CProcessWatchThread

IMPLEMENT_DYNCREATE(CProcessWatchThread, CWinThread)

CProcessWatchThread::CProcessWatchThread()
{
	m_hNotifyWindow = nullptr;
	m_bCancel = false;
	m_bReset = FALSE;
}

CProcessWatchThread::~CProcessWatchThread()
{
}

BOOL CProcessWatchThread::InitInstance()
{
	DWORD dwTargetCount = 0;
	while (!m_bCancel)
	{
		DWORD aProcessIds[2048] = {0};
		DWORD dwProcessCount = 0;
		if (EnumProcesses(aProcessIds, 2048, &dwProcessCount))
		{
			DWORD dwCurCount = 0;
			for (DWORD i = 0; i < dwProcessCount; ++i)
			{
				HANDLE hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, aProcessIds[i]);
				if (hProcess)
				{
					TCHAR cExeName[MAX_PATH] = {0};
					if (GetModuleBaseName(hProcess, nullptr, cExeName, MAX_PATH))
						if (_tcsicmp(cExeName, strEXE) == 0)
						{
							++ dwCurCount;
							CString strPath = strDLL;
							LPVOID lpBuffer = VirtualAllocEx(hProcess, nullptr, (strPath.GetLength() + 1) * sizeof(TCHAR), MEM_COMMIT, PAGE_READWRITE);
							if (lpBuffer)
							{
								DWORD dwWritten;
								WriteProcessMemory(hProcess, lpBuffer, strPath.GetString(), (strPath.GetLength() + 1) * sizeof(TCHAR), &dwWritten);
								DWORD dwThreadId;
								HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, lpBuffer, 0, &dwThreadId);
								if (hThread)
								{
									WaitForSingleObject(hThread, INFINITE);
									CloseHandle(hThread);
								}
								VirtualFreeEx(hProcess, lpBuffer, 0, MEM_DECOMMIT);
							}
// 							while (WaitForSingleObject(hProcess, 0) == WAIT_TIMEOUT && !m_bCancel)
// 								Sleep(1000);
						
							m_bCancel = false;
						}
					CloseHandle(hProcess);
				}
			}
			if ( dwCurCount > dwTargetCount)
			{
				dwTargetCount = dwCurCount;
				m_bReset = FALSE;
				while ( ! m_bCancel && ! m_bReset)
					Sleep(1000);
			}
			dwTargetCount = dwCurCount;
		}
		Sleep(1000);
	}
	return FALSE;
}

void CProcessWatchThread::ReSet()
{
	m_bReset = TRUE;
}

int CProcessWatchThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProcessWatchThread, CWinThread)
END_MESSAGE_MAP()

CProcessWatchThread * CProcessWatchThread::NewInstance()
{
	return (CProcessWatchThread *)AfxBeginThread(RUNTIME_CLASS(CProcessWatchThread), 0, 0, CREATE_SUSPENDED, nullptr);
}

void CProcessWatchThread::StartWatch()
{
	ResumeThread();
}

void CProcessWatchThread::CancelWatch()
{
	m_bCancel = true;
}

void CProcessWatchThread::SetNotifyWindow( HWND hWnd )
{
	m_hNotifyWindow = hWnd;
}
// CProcessWatchThread message handlers
