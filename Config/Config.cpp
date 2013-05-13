#include "StdAfx.h"
#include "Config.h"
#include "../SharedMemory/SharedMemroy.h"

#define CONFIG_NAME				_T("config.ini")
static CString g_strConfigPath;

CConfig::CConfig(void)
{
}


CConfig::~CConfig(void)
{
}

CString CConfig::GetLocalDir()
{
	CString strDir = GetConfigPath();
	strDir.TrimRight(CONFIG_NAME);
	return strDir;
}
CString CConfig::GetConfigPath()
{
	if (g_strConfigPath.IsEmpty())
	{
		CSharedMemroy sm;
		g_strConfigPath = sm.GetClientPath();
		g_strConfigPath.TrimRight(_T("\\"));
		g_strConfigPath += _T("\\");
		g_strConfigPath += CONFIG_NAME;
	}
	return g_strConfigPath;
}
void CConfig::GetQuickStartParam(__out LPTSTR lpBuff, int nSize)
{
	GetPrivateProfileString(_T("MAIN"), _T("QSPARAM"), _T(""), lpBuff, nSize, GetConfigPath());
}
void CConfig::SetQuickStartParam(__in LPTSTR lpParam)
{
	WritePrivateProfileString(_T("MAIN"), _T("QSPARAM"), lpParam, GetConfigPath());
}
void CConfig::GetGameDir(__out LPTSTR lpBuff_, int nSize)
{
	GetPrivateProfileString(_T("MAIN"), _T("GameDir"), _T(""), lpBuff_, nSize, GetConfigPath());
}
void CConfig::SetGameDir(__in LPTSTR lpDir_)
{
	WritePrivateProfileString(_T("MAIN"), _T("GameDir"), lpDir_, GetConfigPath());
}
BOOL CConfig::IsNoLoginMusicEnabled()
{
	int nRet = GetPrivateProfileInt(_T("CONFIG"), _T("NOLOGMUSIC"), TRUE, GetConfigPath());
	return (BOOL)nRet;
}
void CConfig::EnalbeNoLoginMusic(BOOL bEnable)
{
	TCHAR tsz[2] = {0};
	tsz[0] = bEnable+0x30;
	WritePrivateProfileString(_T("CONFIG"), _T("NOLOGMUSIC"), tsz, GetConfigPath());
}

BOOL CConfig::IsNoAdEnabled()
{
	int nRet = GetPrivateProfileInt(_T("CONFIG"), _T("NOAD"), TRUE, GetConfigPath());
	return (BOOL)nRet;
}
void CConfig::EnalbeNoAd(BOOL bEnable)
{
	TCHAR tsz[2] = {0};
	tsz[0] = bEnable+0x30;
	WritePrivateProfileString(_T("CONFIG"), _T("NOAD"), tsz, GetConfigPath());
}

void CConfig::SendMsgToMainDlg(WPARAM type, LPARAM code/* = 0*/, UINT uMsg/* = ENM_MSG_DLL*/)
{
	CSharedMemroy sm;
	HWND hWnd = sm.GetMainWindow();
	if (IsWindow(hWnd))
	{
		SendMessage(hWnd, uMsg, type, code);
	}
}
void CConfig::PostMsgToMainDlg(WPARAM type, LPARAM code/* = 0*/, UINT uMsg/* = ENM_MSG_DLL*/)
{
	CSharedMemroy sm;
	HWND hWnd = sm.GetMainWindow();
	if (IsWindow(hWnd))
	{
		PostMessage(hWnd, uMsg, type, code);
	}
}

static BOOL g_bVisible = TRUE;
BOOL CALLBACK GetCurrentWndProc(
	HWND hwnd,
	LPARAM lParam
	)
{
	BOOL  Result=TRUE;
	DWORD dwPid = 0;
	if (GetParent(hwnd) == NULL && IsWindowVisible(hwnd) == g_bVisible)
	{
		GetWindowThreadProcessId(hwnd, &dwPid);
		if (GetCurrentProcessId() == dwPid)
		{
			*(HWND*)lParam = hwnd;
			Result = FALSE;
		}
	}
	return Result;
}
HWND CConfig::GetCurrentWindow(BOOL bVisible /*= TRUE*/)
{
	HWND hWnd = NULL;
	g_bVisible = bVisible;
	EnumWindows(GetCurrentWndProc, (LPARAM)&hWnd);
	return hWnd;
}

//////////////////////////////////////////////////////////////////////////
// 物品列表相关

BOOL CConfig::IsItemBlackModule()
{
	int nRet = GetPrivateProfileInt(_T("ITEMABOUT"), _T("IsBlackTable"), FALSE, GetConfigPath());
	return (BOOL)nRet;
}
void CConfig::SetItemFilterModule( BOOL bBlack /*= FALSE*/ )
{
	TCHAR tsz[2] = {0};
	tsz[0] = bBlack+0x30;
	WritePrivateProfileString(_T("ITEMABOUT"), _T("IsBlackTable"), tsz, GetConfigPath());
}
void CConfig::SetItemTable(std::vector<std::string>& table)
{
	USES_CONVERSION;
	CString strName;
	int nCount = table.size();
	for (int i = 0; i < nCount; ++i )
	{
		strName.Format(_T("ITEAM%02d"), i);
		WritePrivateProfileString(_T("ITEMANAME"), strName, A2T(table[i].c_str()), GetConfigPath());
	}
}
void CConfig::GetItemTable(std::vector<std::string>& table)
{
	USES_CONVERSION;
	CString strName;
	TCHAR tszTmp[MAX_PATH];
	int i = 0;
	do 
	{
		ZeroMemory(tszTmp, MAX_PATH*sizeof(TCHAR));
		strName.Format(_T("ITEAM%02d"), i);
		GetPrivateProfileString(_T("ITEMANAME"), strName, _T(""), tszTmp, MAX_PATH, GetConfigPath());
		if ( 0 == tszTmp[0] )
		{
			break;
		}
		table.push_back(T2A(tszTmp));
		++ i;
	} while (TRUE);
}