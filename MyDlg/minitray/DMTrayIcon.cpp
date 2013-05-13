#include "StdAfx.h"
#include "DMTrayIcon.h"

CDMTrayIcon::CDMTrayIcon(void)
{
	m_pDlgWnd = NULL;
	m_pMenu = NULL;
}

CDMTrayIcon::~CDMTrayIcon(void)
{
}

void CDMTrayIcon::CreateTrayIcon(CWnd* DlgWnd, UINT uID, HICON hIcon, LPTSTR lpTip)
{
	m_pDlgWnd = DlgWnd;
	ZeroMemory(&m_notifyIcon, sizeof(NOTIFYICONDATA));
	m_notifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_notifyIcon.hWnd = DlgWnd->m_hWnd;
	m_notifyIcon.uID = uID;
	m_notifyIcon.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
	m_notifyIcon.uCallbackMessage = WM_NOTIFYICON;
	m_notifyIcon.hIcon = hIcon;
	m_notifyIcon.dwState = NIS_SHAREDICON;
	//wcscpy(m_notifyIcon.szInfo, L"开始运行...");	//如果赋初始值在Create时会直接弹气泡
	m_notifyIcon.uTimeout = 10000;	//minimum and maximum timeout values are currently set at 10 seconds and 30 seconds
	m_notifyIcon.uVersion = NOTIFYICON_VERSION;
	//wcscpy(m_notifyIcon.szInfoTitle, L"提示");		//Title
	m_notifyIcon.dwInfoFlags = NIIF_INFO;
	_tcscpy_s(m_notifyIcon.szTip, sizeof(m_notifyIcon.szTip) / sizeof(TCHAR), lpTip);	//Tip
	Shell_NotifyIcon(NIM_ADD, &m_notifyIcon);
}

void CDMTrayIcon::DeleteTrayIcon()
{
	Shell_NotifyIcon(NIM_DELETE, &m_notifyIcon);
}

void CDMTrayIcon::FixTrayIconPropertiy(TRAYICONPROPERTIES type, DWORD dwValue)
{
	switch (type)
	{
	case ENM_FLAGS:
		m_notifyIcon.uFlags = dwValue;
		break;
	case ENM_UID:
		m_notifyIcon.uID = dwValue;
		break;
	case ENM_HICON:
		m_notifyIcon.hIcon = (HICON)dwValue;
		break;
	case ENM_STATE:
		m_notifyIcon.dwState = dwValue;
		break;
	case ENM_VERSION:
		m_notifyIcon.uVersion = dwValue;
		break;
	}
	ZeroMemory(m_notifyIcon.szInfo, sizeof(m_notifyIcon.szInfo));	//去掉内容，即修改属性时不会弹气泡
	Shell_NotifyIcon(NIM_MODIFY, &m_notifyIcon);
}

void CDMTrayIcon::MiniToTray()
{
	ASSERT(m_pDlgWnd);
	m_pDlgWnd->ShowWindow(SW_HIDE);
}

void CDMTrayIcon::TrayToNormal()
{
	ASSERT(m_pDlgWnd);
	m_pDlgWnd->ShowWindow(SW_NORMAL);
}

void CDMTrayIcon::SetPopMenu(CMenu* pMenu)
{
	m_pMenu = pMenu;
}

void CDMTrayIcon::ShowPopMenu()
{
	ASSERT(m_pMenu);
	ASSERT(m_pDlgWnd);
	CPoint pt;
	GetCursorPos(&pt);//获取鼠标当前的坐标
	m_pMenu->TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, m_pDlgWnd);
}

void CDMTrayIcon::ShowBalloon(LPTSTR lpTitle, LPTSTR lpContent, DWORD dwFlag/* = NIIF_INFO*/, DWORD dwTimeOut/* = 0*/)
{
	_tcscpy_s(m_notifyIcon.szInfoTitle, sizeof(m_notifyIcon.szInfoTitle) / sizeof(TCHAR), lpTitle);
	_tcscpy_s(m_notifyIcon.szInfo, sizeof(m_notifyIcon.szInfo) / sizeof(TCHAR), lpContent);
	m_notifyIcon.dwInfoFlags = dwFlag;
	if (dwTimeOut)
		m_notifyIcon.uTimeout = dwTimeOut;
	Shell_NotifyIcon(NIM_MODIFY, &m_notifyIcon);
}