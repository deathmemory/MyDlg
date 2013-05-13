#pragma once
#include <atlstr.h>
#include <vector>

enum ENM_SENDMSGTOMAIN
{
	WM_MSG_DLL	= (WM_USER + 200),
	WM_FOUND_ITEM ,
};

class CConfig
{
	CConfig(void);
	virtual ~CConfig(void);
public:
	static CString GetLocalDir();
	static CString GetConfigPath();

	static void GetQuickStartParam(__out LPTSTR lpBuff, int nSize);
	static void SetQuickStartParam(__in LPTSTR lpParam);

	static void GetGameDir(__out LPTSTR lpBuff_, int nSize);
	static void SetGameDir(__in LPTSTR lpDir_);

	static BOOL IsNoLoginMusicEnabled();
	static void EnalbeNoLoginMusic(BOOL bEnable);

	static BOOL IsNoAdEnabled();
	static void EnalbeNoAd(BOOL bEnable);

	static void SendMsgToMainDlg(WPARAM type, LPARAM code = 0, UINT uMsg = WM_MSG_DLL);
	static void PostMsgToMainDlg(WPARAM type, LPARAM code = 0, UINT uMsg = WM_MSG_DLL);
	
	static HWND GetCurrentWindow(BOOL bVisible = TRUE);
	
	//////////////////////////////////////////////////////////////////////////
	// 物品列表相关

	static BOOL IsItemBlackModule();
	static void SetItemFilterModule( BOOL bBlack = FALSE );

	static void SetItemTable(std::vector<std::string>& table);
	static void GetItemTable(std::vector<std::string>& table);

};

