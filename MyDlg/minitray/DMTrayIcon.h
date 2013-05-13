/************************************************************************/
/* 主要功能: 托盘封装
/* 注意事项:
/*	1. targetver.h 宏定义,降低版本要求,以适应低版本系统响应消息
	/*
	#ifndef WINVER                          // 指定要求的最低平台是 Windows Vista。
	//#define WINVER 0x0600           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
	#define WINVER 0x0501           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
	#endif

	#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
	//#define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
	#define _WIN32_WINNT 0x0501     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
	#endif

	#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
	#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
	#endif

	#ifndef _WIN32_IE                       // 指定要求的最低平台是 Internet Explorer 7.0。
	//#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
	#define _WIN32_IE 0x0600        // 将此值更改为相应的值，以适用于 IE 的其他版本。
	#endif

	*/
/*	2. 主Dlg 程序加入消息循环
	ON_MESSAGE(WM_NOTIFYICON, OnNotifyIcon) 
	WM_NOTIFYICON 为 Tray 自定义消息
	OnNotifyIcon 为主Dlg 接收消息响应回调函数

/* 作者: 解云飞
/* 日期: 2012/06/26
/************************************************************************/
#pragma once
#define WM_NOTIFYICON			 (WM_USER + 4000)

class CDMTrayIcon
{
	enum TRAYICONPROPERTIES
	{
		ENM_FLAGS,
		ENM_UID,
		ENM_HICON,
		ENM_STATE,
		ENM_VERSION
	};
public:
	CDMTrayIcon(void);
	~CDMTrayIcon(void);

	void CreateTrayIcon(CWnd* DlgWnd, UINT uID, HICON hIcon, LPTSTR lpTip);
	void DeleteTrayIcon();
	void FixTrayIconPropertiy(TRAYICONPROPERTIES type, DWORD dwValue);
	void MiniToTray();
	void TrayToNormal();
	void SetPopMenu(CMenu* pMenu);
	void ShowPopMenu();
	void ShowBalloon(LPTSTR lpTitle, LPTSTR lpContent, DWORD dwFlag = NIIF_INFO, DWORD dwTimeOut = 0);
private:
	NOTIFYICONDATA m_notifyIcon;
	CWnd* m_pDlgWnd;
	CMenu* m_pMenu;
};
