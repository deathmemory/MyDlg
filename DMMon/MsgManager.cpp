#include "StdAfx.h"
#include "MsgManager.h"
#include "../Config/Config.h"
#include "SearchCodeManager.h"

#define MSG_WAIT_TIME				100

#define CHECK_SEARCHCODE_INIT()	if ( ! g_SCManagerEx.IsInit() ) \
	g_SCManagerEx.Init();
#define GETKEYSTATEEX(vkey)				(0xf000 & GetKeyState(vkey))


HWND g_hWnd = NULL;
BOOL CMsgManager::g_bWatch = TRUE;
CMsgManager::CMsgManager(void)
{
	m_hMsgTread = NULL;
}


CMsgManager::~CMsgManager(void)
{
}

BOOL CMsgManager::Init(DWORD dwAddr/* = TRUE*/)
{
	if (CBaseHook::Init(dwAddr))
	{
		g_hWnd = CConfig::GetCurrentWindow();
		if ( ! m_hMsgTread )
			m_hMsgTread = CreateThread(NULL, 0, MsgThread, this, 0, NULL);
		return TRUE;
	}
	return FALSE;
}
VOID CMsgManager::UnInit()
{
	if (m_hMsgTread)
	{
		g_bWatch = FALSE;
		WaitForSingleObject(m_hMsgTread, MSG_WAIT_TIME);
		CloseHandle(m_hMsgTread);
		m_hMsgTread = NULL;
	}
	if (g_SCManagerEx.IsInit())
	{
		g_SCManagerEx.UnInit();
	}
}

HANDLE g_hThread = NULL;
DWORD WINAPI RecoverTestThread(LPVOID)
{
	while (true)
	{
		g_AutoCommit.RecoverTest();
		g_RecvMsgManager.AddMessage("committing ...", mclr_green, mclr_black);
		Sleep(100);
	}
}

DWORD CMsgManager::MsgThread(LPVOID)
{
	while ( g_bWatch )
	{	//如果是当前窗体才执行
		if (!IsWindow(g_hWnd))
		{
			g_hWnd = CConfig::GetCurrentWindow(); 
		}
		if (GetForegroundWindow() == g_hWnd)
		{
			BOOL bControl = GETKEYSTATEEX(VK_CONTROL);
			if (bControl)
			{
				CHECK_SEARCHCODE_INIT();
				if ( GETKEYSTATEEX('1') )
				{
					g_CallHero.CallHero(enm_hero_soldier);
				}
				else if ( GETKEYSTATEEX('2') )
				{
					g_CallHero.CallHero(enm_hero_master);
				}
				else if ( GETKEYSTATEEX('3') )
				{
					g_CallHero.CallHero(enm_hero_priest);
				}
				else if ( GETKEYSTATEEX('6') )
				{
					if ( ! g_AutoCommit.IsRecoverInit() )
						g_AutoCommit.InitRecover();
					g_AutoCommit.GetCurItem();
				}
				else if ( GETKEYSTATEEX('7') )
				{
					if ( ! g_hThread )
						g_hThread = CreateThread(NULL, 0, RecoverTestThread, NULL, 0, NULL);
					else
					{
						SuspendThread(g_hThread);
						TerminateThread(g_hThread, 0);
						CloseHandle(g_hThread);
						g_hThread = NULL;
						g_RecvMsgManager.AddMessage("stop committing");
					}
				}
				else if ( GETKEYSTATEEX('8') )
				{
					static BOOL bScan = FALSE;
					if ( ! bScan )
					{
						g_MapItemMoniter.ResumeScanItems();
						g_RecvMsgManager.AddMessage("start scan items ...");
						bScan = TRUE;
					}
					else
					{
						g_MapItemMoniter.StopScanItems();
						g_RecvMsgManager.AddMessage("stop scan items");
						bScan = FALSE;
					}
				}
				else if ( GETKEYSTATEEX('9') )
				{
					
				}
				else if ( GETKEYSTATEEX('0') )
				{	// 强制重新 Init
					g_SCManagerEx.Init();
				}
				else if ( GETKEYSTATEEX('Z') )
				{
					g_RecvMsgManager.AddMessage("this is a test", mclr_green, mclr_black);
					g_CallHero.CallHero(enm_hero_soldier, enm_hero_callback, enm_hero_character_main);
				}
			}
			
		}
		Sleep(MSG_WAIT_TIME);
	}
	return  0;
}