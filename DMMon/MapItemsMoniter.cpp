#include "StdAfx.h"
#include "MapItemsMoniter.h"
#include "PersonManager.h"
#include "PepoleMove.h"
#include "SearchCodeManager.h"
#include "../publicsrc/BaseCode/hook/inline/HookInternal.h"
#include "../Config/Config.h"
#include <math.h>
static CMapItemsMoniter* g_pMapItemsMoniter = NULL;
static BOOL g_bMoveThread = TRUE;

CMapItemsMoniter::CMapItemsMoniter(void)
{
	g_pMapItemsMoniter = this;
	m_hScanThread = NULL;
}


CMapItemsMoniter::~CMapItemsMoniter(void)
{
}

BOOL CMapItemsMoniter::Init(DWORD dwAddr/* = 0*/)
{
	BOOL bRet = CBaseHook::Init(dwAddr);
	if (bRet)
	{
		m_bBlack = CConfig::IsItemBlackModule();
		m_vctTable.clear();
		CConfig::GetItemTable(m_vctTable);
		g_bMoveThread = TRUE;
		if (!m_hScanThread)
			m_hScanThread = CreateThread(NULL, 0, ScanItemThread, this, 0, NULL);
		if (m_hScanThread)
			SuspendThread(m_hScanThread);
	}
	return bRet;
}

VOID CMapItemsMoniter::UnInit()
{
	StopScanItems();
	if (m_hScanThread)
	{
		TerminateThread(m_hScanThread, 0);
		m_hScanThread = NULL;
	}
	
}

DWORD CMapItemsMoniter::ScanItemThread(LPVOID)
{
	DWORD dwpItemAddr = g_MapItemMoniter.GetAddr();
	if (! dwpItemAddr) 
		return -1;
	CHAR szName[MAX_PATH] = {0};
	while(g_bMoveThread)
	{
		if ( ! IsBadReadPtr((DWORD*)(dwpItemAddr), 4) )
		{
			BOOL bFound = FALSE;
			DWORD dwCount = *(DWORD*)(dwpItemAddr+0x08);
			for ( int i = 0; i < dwCount; ++ i )
			{
				DWORD dwBaseAddr = *(DWORD*)(*(DWORD*)(dwpItemAddr+4)+i*4);
				USHORT itemX = *(USHORT*)((USHORT*)(dwBaseAddr+4));
				USHORT itemY = *(USHORT*)(((USHORT*)(dwBaseAddr+4))+1);
				CHAR* lpItemName = (CHAR*)(*(DWORD*)(*(DWORD*)(dwpItemAddr+4)+i*4)+0x10);
				if ( ! IsBadReadPtr(lpItemName, 4))	//当列表清空时， 这里会 Bad Ptr， 所以要加判断
				{
					UINT uNameLen = *(lpItemName);
					ZeroMemory(szName, MAX_PATH);
					memcpy(szName, lpItemName+1, uNameLen);
					if (! g_MapItemMoniter.m_bBlack )
					{
						int nCount = g_MapItemMoniter.m_vctTable.size();
						for (int j = 0; j < nCount; ++j )
						{
							string strCurVItem = g_MapItemMoniter.m_vctTable[j];
							if (memcmp(szName, strCurVItem.c_str(), strCurVItem.length()) == 0 )
							{
								bFound = TRUE;
								CConfig::PostMsgToMainDlg(WM_FOUND_ITEM);	// 发现物品提示
								g_MapItemMoniter.ItemPrint(szName, itemX, itemY);
								// 下面开始自动拾取动作
								//g_PepoleMove.MoveTo(itemY, itemX);
							}
						}
					}
				}
			}
// 			if ( ! bFound /*&& isBackPos*/)	// 是否坚守岗位
// 			{
// 				g_PepoleMove.MoveTo(13, 11);
// 				//g_PepoleMove.MoveTo(249, 401);
// 			}
		}
		
		Sleep(500);
	}
	return 0;
}

VOID CMapItemsMoniter::ItemPrint(CHAR* pszName, USHORT x, USHORT y)
{
	CHAR *pszflag = 0;
	CHAR szContext[MAX_PATH] = {0};
	USHORT px = g_PersonManager.m_pperson->pos.uX;
	USHORT py = g_PersonManager.m_pperson->pos.uY;
	if ( x <= px && y <= py)
		pszflag = "↖";
	else if ( x <= px && y == py )
		pszflag = "←";
	else if ( x <= px && y >= py )
		pszflag = "↙";
	else if ( x == px && y >= py )
		pszflag = "↓";
	else if ( x >= px && y >= py )
		pszflag = "↘";
	else if ( x >= px && y == py )
		pszflag = "→";
	else if ( x >= px && y <= py )
		pszflag = "↗";
	else //if ( x == px && y <= py )
		pszflag = "↑";

	sprintf(szContext, "极品提示:发现->%s, 坐标->%d:%d, %s", pszName, x, y, pszflag);
	g_RecvMsgManager.AddMessage(szContext, mclr_red, mclr_green);

}

VOID CMapItemsMoniter::ResumeScanItems()
{
	if ( m_hScanThread )
		ResumeThread(m_hScanThread);
}

VOID CMapItemsMoniter::StopScanItems()
{
	if ( m_hScanThread )
		SuspendThread(m_hScanThread);
}

