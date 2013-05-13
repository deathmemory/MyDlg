// DMMon.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "DMMon.h"
#include "../Config/Config.h"
#include "MsgManager.h"
#include "SearchCodeManager.h"
#include "../publicsrc/HelpTools/ProcessHelper.hpp"
#include "../publicsrc/publicdef.h"

CMsgManager g_MsgManager;

void InitHookApi(DWORD dw/* = 0*/)
{
//  	g_SCManagerEx.SearchRecvMsgAddr();
//  	if (FALSE == g_RecvMsgManager.Init(g_SCManagerEx.GetRecvMsgAddr()))
//  	{
//  		MessageBoxA(0, "g_RecvMsgManager init", 0, 0);
//  	}
	if (FALSE == g_MsgManager.Init())
	{
		MessageBoxA(0, "g_MsgManager init", 0, 0);
	}		
}

void ExitHookApi()
{
	if ( g_MsgManager.IsInit() )
		g_MsgManager.UnInit();

	if (CConfig::IsNoAdEnabled())
	{
		BOOL bRet = FALSE;
		CProcessHelper ph;
		if (ph.PromoteLimit())
			bRet = ph.KillProcByName(STR_TARGET_NAME);
		if ( ! bRet )
		{
			DWORD dwErr = 0;
			dwErr = GetLastError();
			TCHAR tszMsg[MAX_PATH*2] = {0};
			FormatMessage(
				FORMAT_MESSAGE_FROM_SYSTEM, 
				NULL,
				dwErr,
				0,
				tszMsg,
				MAX_PATH*2,
				NULL
				);
			MessageBox(0, tszMsg, NULL, NULL);
		}
	}
}
