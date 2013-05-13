
// MyDlgDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MyDlg.h"
#include "MyDlgDlg.h"
#include "afxdialogex.h"

//#include "ProcessHelper.h"
//#include "../SharedMemory/SharedMemroy.h"
#include "../publicsrc/publicdef.h"
#include "../Config/Config.h"
#include "ProcessWatchThread.h"
#include "WaitForObjectsThread.h"
#include "../publicsrc/HelpTools/ProcessHelper.hpp"

#include <Winsvc.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMyDlgDlg dialog
extern CMyDlgApp theApp;
//CSharedMemroy m_ShareMemory;

CMyDlgDlg::CMyDlgDlg(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CMyDlgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pBtnMultLog = NULL;	//多开
}

void CMyDlgDlg::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyDlgDlg, CSkinDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_NCHITTEST()
	ON_WM_TIMER()
	ON_MESSAGE(WM_NOTIFYICON, &CMyDlgDlg::OnTrayNotify)
	ON_COMMAND(ID_STARTGAME, &CMyDlgDlg::OnMnuStartgame)
	ON_COMMAND(ID_MAIN_EXIT, &CMyDlgDlg::OnMnuExit)
END_MESSAGE_MAP()


// CMyDlgDlg message handlers

BOOL CMyDlgDlg::OnInitDialog()
{
	CSkinDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	theApp.m_ShareMemory.SetMainWindow(GetSafeHwnd());
	theApp.m_ShareMemory.SetClientPath(GetLocalDir_());
	if ( ! RegisterHotKey(GetSafeHwnd(), 1, MOD_CONTROL | MOD_SHIFT /*| MOD_NOREPEAT*/, VK_OEM_3))	//VK_OEM_3 => '`~'		#if(WINVER >= 0x0601)#define MOD_NOREPEAT    0x4000
		MessageBox(_T("RegisterHotKey failed !"), _T("Error"));
//  	CUIMessageBox::SetCloseBtnPos(8, 8);
//  	CUIMessageBox::SetCommBtnSize(86, 26);
//  	CUIMessageBox::SetBtnTextColor(RGB(0x22, 0x57, 0x66),RGB(0x3f, 0xa3, 0xbf), RGB(0x22, 0x57, 0x66), RGB(0x41, 0x4c,0x4f));
//  	CUIMessageBox::SetTitleBarHeight(31);
//  	CUIMessageBox::SetSpaceSpan(25);
//  	CUIMessageBox::SetTitlePos(42, 12);
//  	CUIMessageBox::SetTitleTextColor(RGB(149, 203, 230));
//  	CUIMessageBox::SetInfoTextColor(RGB(0x58, 0x6d, 0x78));
//  	CUIMessageBox::SetTitleTextStyle(DT_LEFT);
	m_pWatch_Mir1_dat_Thread = CProcessWatchThread::NewInstance();
	m_pWatch_Mir1_dat_Thread->strDLL = GetLocalDir_() + INJECTION_DLL_NAME;
	m_pWatch_Mir1_dat_Thread->strEXE = INJECTION_EXE_NAME;
	m_pWatch_Mir1_dat_Thread->StartWatch();

	SetWindowText(STR_MAIN_TITLE);
	SetWindowPos(nullptr, 0, 0, DLG_MAIN_WIDTH, DLG_MAIN_HEIGHT, SWP_NOZORDER|SWP_NOMOVE);
	ModifyStyle(0,WS_SYSMENU|WS_MINIMIZEBOX,0);
	CenterWindow();

	SetCommonCheckBoxSkin(IDB_CHK_SKIN);

	SetSkinImage(IDB_MAINDLG_NORMAL, IDB_MAINDLG_HOVER, IDB_MAINDLG_HIT, IDB_MAINDLG_DISABLE);
	InitControls();
	SetupRegion(5);	// this call muest be last

	m_dmTrayIcon.CreateTrayIcon(this, IDR_MAINFRAME, m_hIcon, STR_MAIN_TITLE);
	m_dmTrayIcon.ShowBalloon(_T("欢迎使用"), STR_MAIN_TITLE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMyDlgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CSkinDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMyDlgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMyDlgDlg::InitControls()
{
	CRect rect;
	GetClientRect(&rect);
	m_UI->SetPos(rect);

	CUILabel *lpLab=NULL;
	CUIButton *lpBtn = NULL;
	CRect rc = CRect(DLG_MAIN_WIDTH-47, 0, DLG_MAIN_WIDTH, 22);
	lpBtn = RegiserButton(ID_BTN_CLOSE, rc);
	if (lpBtn)
	{
		lpBtn->SetImage( GetLocalDir_() + _T("img\\btn_close2-47x22.png") );

		this->AddNotDragArea(rc);
	}
	rc = CRect(rc.left-33, 0, rc.right-47, 22);
	lpBtn = RegiserButton(ID_BTN_MIN, rc);
	if (lpBtn)
	{
		lpBtn->SetImage( GetLocalDir_() + _T("img\\btn_min2-33x22.png") );

		this->AddNotDragArea(rc);
	}
	/*rc = CRect(rc.left-33, 0, rc.right-33, 22);*/
	rc = CRect(DLG_MAIN_WIDTH-200-20, 50, DLG_MAIN_WIDTH-20, 50+80);
	lpBtn = RegiserButton(ID_BTN_START, rc/*, _T("开始游戏")*/);
	if (lpBtn)
	{
		lpBtn->SetImage( GetLocalDir_() + _T("img\\btn-startgame2-5x2.png") );           

		this->AddNotDragArea(rc);
	}
// 	rc = CRect(rc.left, rc.bottom, rc.right, rc.bottom+20);
// 	lpLab = RegiserLabel(ID_LAB_RECORD, rc, _T("<a>配置快捷启动</a>"));
// 	if (lpLab)
// 	{
// 		lpLab->SetTextStyle(DT_CENTER);
// 		lpLab->SetImageAsLinker(true);
// 
// 		this->AddNotDragArea(rc);
// 	}
	rc = CRect(rc.left, rc.bottom+10, rc.right+50, rc.bottom+10+14);
	m_pBtnChkLogMusic = RegiserButton(ID_CHK_LOGMUSIC, rc, _T("关闭登录音乐"));
	if (m_pBtnChkLogMusic)
	{
		m_pBtnChkLogMusic->SetButtonType(BUTTON_TYPE_CHECKBOX);
		m_pBtnChkLogMusic->SetCheck( CConfig::IsNoLoginMusicEnabled() ); 
		//lpBtn->SetImage(_T("img\\checkBox-14x14.png"));

		this->AddNotDragArea(rc);
	}

	rc = CRect(rc.left, rc.bottom+10, rc.right+50, rc.bottom+10+14);
	m_pBtnChkAD = RegiserButton(ID_CHK_AD, rc, _T("进入游戏时关闭Air【退出时无广告】"));
	if (m_pBtnChkAD)
	{
		m_pBtnChkAD->SetButtonType(BUTTON_TYPE_CHECKBOX);
		m_pBtnChkAD->SetCheck( CConfig::IsNoAdEnabled() ); 
		//lpBtn->SetImage(_T("img\\checkBox-14x14.png"));

		this->AddNotDragArea(rc);
	}

	rc = CRect(rc.left, rc.bottom+10, rc.right+50, rc.bottom+10+14);
	m_pBtnMultLog = RegiserButton(ID_BTN_MULTILOG, rc, _T("多开游戏"));
	if (m_pBtnMultLog)
	{
		//m_pBtnMultLog->SetImage( GetLocalDir_() + _T("img\\btn-startgame2-5x2.png") );   
		m_pBtnMultLog->SetButtonType(BUTTON_TYPE_CHECKBOX);
		m_pBtnMultLog->SetCheck( CheckMultiLog() ); 
		//lpBtn->SetImage(_T("img\\checkBox-14x14.png
		this->AddNotDragArea(rc);
	}
// 	CRect lrcUIButton = CRect(550, 449, 655, 566);
// 	lpBtn = RegiserButton(ID_BTN_START, lrcUIButton);
// 	if (lpBtn)
// 	{
// 		POINT pt[] = {
// 			{lrcUIButton.left, lrcUIButton.top+27},
// 			{lrcUIButton.left+50, lrcUIButton.top},
// 			{lrcUIButton.right, lrcUIButton.top+27},
// 			{lrcUIButton.right, lrcUIButton.bottom-30},
// 			{lrcUIButton.left+50, lrcUIButton.bottom},
// 			{lrcUIButton.left, lrcUIButton.bottom-30}
// 		};
// 		lpBtn->SetAreaPoint(pt, 6);
// 		this->AddNotDragArea(lrcUIButton);
// 	}

}

void CMyDlgDlg::Notify(TNotifyUI& msg)
{
	switch (msg.nType)
	{
	case UI_NOTIFY_CLICK:
		switch (msg.pSender->GetId())
		{
		case ID_BTN_CLOSE:
			this->OnBtnClickClose();
			break;
		case ID_BTN_MIN:
			this->OnBtnClickMin();
			break;
		case ID_BTN_START:
			this->OnBtnClickStartGame();
			break;
		case ID_CHK_LOGMUSIC:
			CConfig::EnalbeNoLoginMusic( m_pBtnChkLogMusic->IsChecked() );
			break;
		case ID_CHK_AD:
			CConfig::EnalbeNoAd( m_pBtnChkAD->IsChecked() );
			break;
		case ID_BTN_MULTILOG:
			this->OnBtnClickMultiLog();
			break;
		}
		break;
	case UI_NOTIFY_LINK:
		switch (msg.pSender->GetId())
		{
		case ID_LAB_RECORD:
			this->OnBtnClickSetQuickStart();
			break;
		}
		break;
	}
}

CString CMyDlgDlg::GetLocalDir_()
{
	if (m_strLocalDir_.IsEmpty())
	{
		TCHAR szModulePath[MAX_PATH] = {0};
		GetModuleFileName(nullptr, szModulePath, MAX_PATH);

		m_strLocalDir_ = szModulePath;
		int nPos = m_strLocalDir_.ReverseFind(_T('\\'));
		if (nPos != -1)
		{
			++nPos;
			m_strLocalDir_.Delete(nPos, m_strLocalDir_.GetLength() - nPos);
		}
	}
	return m_strLocalDir_;
}

LRESULT CMyDlgDlg::OnNcHitTest(CPoint point)
{
	// 实现对话框拖拽
	if ( ! this->IsNotDragArea(point))
		return HTCAPTION;
	return CSkinDialog::OnNcHitTest(point);
}

void CMyDlgDlg::AddNotDragArea(CRect rc)
{
	m_rcNotDragArea.push_back(rc);
}

BOOL CMyDlgDlg::IsNotDragArea(POINT point)
{
	int nNum = m_rcNotDragArea.size();
	for (int i = 0; i < nNum; ++ i)
	{
		CRect rc = m_rcNotDragArea[i];
		ClientToScreen(&rc);
		if ( rc.PtInRect(point) )
			return TRUE;
	}
	return FALSE;
}

BOOL CMyDlgDlg::SetDlgSkinFromFile(LPTSTR lpFileName)
{
	return TRUE;
}


LRESULT CMyDlgDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
 	switch (message)
 	{
 	case WM_HOTKEY:
 		{
			HWND hMir2LoadProcWnd = ::FindWindow(MIR2_LOADPROC_CLASS, MIR2_LOADPROC_TITLE);
			//HWND hMir2LoadProcWnd = ::FindWindow(_T("Notepad"), NULL);
			if ( IsWindow(hMir2LoadProcWnd) )
			{
				if (::IsWindowVisible(hMir2LoadProcWnd))
					::ShowWindow(hMir2LoadProcWnd, SW_HIDE );
				else
				{
					::ShowWindow(hMir2LoadProcWnd, SW_SHOW );
					::SetForegroundWindow(hMir2LoadProcWnd);
				}
			}
 		}
		break;
	case WM_MSG_DLL:
		{
			m_dmTrayIcon.ShowBalloon(_T("Foud !"), _T("There's a item what you care about !"));
		}
 		break;
 	}
	return CSkinDialog::WindowProc(message, wParam, lParam);
}

void CMyDlgDlg::OnBtnClickMin()
{
	m_dmTrayIcon.MiniToTray();
}

void CMyDlgDlg::OnBtnClickClose()
{
	Exit();
}

void CMyDlgDlg::OnBtnClickStartGame()
{
	//CommandLine = """D:\path\mir1.dat"" /ip:119.147.188.55 /AreaID:125"
	//CString strGamePath = _T("\"D:\\盛大网络\\热血传奇\\mir1.dat\" /ip:119.147.188.55 /AreaID:125");

	CString strGamePath;
	//strGamePath.Format(_T("\"%s%s\" %s"), this->GetGamePath(), STR_WATCH_PROC, tszParam);
	strGamePath.Format(_T("%s%s"), this->GetGamePath(), STR_TARGET_NAME);

	this->AppSetBeforeStartGame();

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	USES_CONVERSION;
	if (CreateProcess(NULL, strGamePath.GetBuffer(strGamePath.GetLength()), 
		NULL, NULL, FALSE, NULL, NULL,NULL, &si, &pi)
		)
	{
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		m_pWatch_Mir1_dat_Thread->ReSet();	// 线程注入重置
		m_dmTrayIcon.MiniToTray();
		m_dmTrayIcon.ShowBalloon( _T("温馨提示"), _T("您可以使用老键控制游戏界面显示/隐藏\n老板键: Ctrl+Shift+~"));
	}
	strGamePath.ReleaseBuffer();
}

void CMyDlgDlg::OnBtnClickSetQuickStart()
{
	MessageBox(_T("通过本程序正常启动一次游戏即可获取快捷启动配置"),
		_T("提示"), MB_OK|MB_ICONINFORMATION
		);
	CString strGamePath = GetGamePath();
	if (! strGamePath.IsEmpty())
	{
		strGamePath += STR_TARGET_NAME;
		STARTUPINFO si = {0};
		PROCESS_INFORMATION pi = {0};
		if (CreateProcess(NULL, strGamePath.GetBuffer(strGamePath.GetLength()),
			NULL, NULL, FALSE, NULL, NULL, NULL, &si,&pi))
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			this->SetTimer(TID_RECORD, 1000, 0);
		}
		strGamePath.ReleaseBuffer();
	}
}

CString CMyDlgDlg::GetGamePath()
{
	if (m_strGameDir_.IsEmpty())
	{
		TCHAR tszGamePath[MAX_PATH] = {0};
		CConfig::GetGameDir(tszGamePath, MAX_PATH);
		m_strGameDir_ = tszGamePath;
	}
	// 如果配置文件中没有则提示用户选择
	if (m_strGameDir_.IsEmpty())
	{
		MessageBox(_T("没有找到游戏路径，请设置游戏路径"), _T("提示"), MB_OK|MB_ICONINFORMATION);
		TCHAR tszFile[MAX_PATH] = {0};
		OPENFILENAME ofn = {0};
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = GetSafeHwnd();
		ofn.lpstrFile = tszFile;
		ofn.nMaxFile = sizeof(tszFile);
		//ofn.lpstrFilter = STR_TARGET_NAME;
		ofn.lpstrFilter = _T("传奇安装目录启动程序(mir.exe)\0mir.exe\0");
		if (GetOpenFileName(&ofn))
		{
			CString temp = ofn.lpstrFile;
			int index = temp.ReverseFind(_T('\\'));
			if (-1 != index)
			{
				m_strGameDir_ = temp.Mid(0, index+1);
				CConfig::SetGameDir(m_strGameDir_.GetBuffer(m_strGameDir_.GetLength()));
				m_strGameDir_.ReleaseBuffer();
			}
		}
	}
	return m_strGameDir_;
}

void CMyDlgDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case TID_RECORD:
		this->GetGameParamTimer();
		break;
	}
	CSkinDialog::OnTimer(nIDEvent);
}

void CMyDlgDlg::GetGameParamTimer()
{
	// add get game param method
	TCHAR tszCmdLine[MAX_PATH] = {0};
	CProcessHelper ph;
	ph.GetProcessCMDLine( STR_WATCH_PROC, tszCmdLine, MAX_PATH );
	if (0 != tszCmdLine[0])
	{
		TCHAR* ptszPos = _tcsstr(tszCmdLine, STR_WATCH_PROC);
		if (ptszPos)
		{
			ptszPos += _tcslen(STR_WATCH_PROC) + 1;
			CConfig::SetQuickStartParam(ptszPos);
			KillTimer(TID_RECORD);
			MessageBox( _T("配置完成!"), _T("完成"), MB_OK|MB_ICONINFORMATION );
		}
	}
}

// 游戏运行先应用各种配置
void CMyDlgDlg::AppSetBeforeStartGame()
{
	CString strGameDir_ = this->GetGamePath();
	if ( CConfig::IsNoLoginMusicEnabled() )
	{
		CString strLoginMusicPatch = strGameDir_ + _T("Wav\\log-in-long2.wav");
		if ( CopyFile(strLoginMusicPatch, strLoginMusicPatch + DM_EXT, FALSE) )
			DeleteFile(strLoginMusicPatch);
	}
	else
	{
		CString strLoginMusicPatch = strGameDir_ + _T("Wav\\log-in-long2.wav");
		if ( CopyFile(strLoginMusicPatch + DM_EXT, strLoginMusicPatch, FALSE) )
			DeleteFile(strLoginMusicPatch + DM_EXT);
	}
}

void CMyDlgDlg::Exit()
{
	CWaitForObjectsThread* pWaitThread = (CWaitForObjectsThread *)AfxBeginThread(RUNTIME_CLASS(CWaitForObjectsThread), 0, 0, CREATE_SUSPENDED, nullptr);
	if (m_pWatch_Mir1_dat_Thread)
	{
		m_pWatch_Mir1_dat_Thread->CancelWatch();
		pWaitThread->AddObject(m_pWatch_Mir1_dat_Thread);
	}
	ResumeThread(pWaitThread->m_hThread);
	WaitForSingleObject(pWaitThread->m_hThread, 2000);
	m_dmTrayIcon.DeleteTrayIcon();
	PostQuitMessage(0);
}

LRESULT CMyDlgDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
	case WM_LBUTTONUP:
		m_dmTrayIcon.TrayToNormal();
		break;
	case WM_RBUTTONUP:
		{
			CMenu menu;
			if (menu.LoadMenu(IDR_MAINMENU))
			{
				CMenu* pPopup = menu.GetSubMenu(0);
				POINT point = {0};
				GetCursorPos(&point);
				//ClientToScreen(&point);
				this->SetForegroundWindow();
				pPopup->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, 
					point.y, this);
			}
		}
		break;
	}
	return S_OK;
}

void CMyDlgDlg::OnMnuStartgame()
{
	// TODO: Add your command handler code here
	this->OnBtnClickStartGame();
}


void CMyDlgDlg::OnMnuExit()
{
	// TODO: Add your command handler code here
	Exit();
}

BOOL CMyDlgDlg::CheckMultiLog()
{
	BOOL bRet = FALSE;
	SC_HANDLE hMgr = OpenSCManager(NULL, NULL, GENERIC_READ);
	if (hMgr)
	{
		SC_HANDLE hSvc = OpenService(hMgr, MULTILOG_SERVICENAME, SERVICE_ALL_ACCESS);
		if (hSvc)
		{
			SERVICE_STATUS Status = {0};
			if ( QueryServiceStatus(hSvc, &Status) )
				if (SERVICE_RUNNING == Status.dwCurrentState)
					bRet = TRUE;
		}
		CloseServiceHandle(hMgr);
	}
	return bRet;
}

void CMyDlgDlg::OnBtnClickMultiLog()
{
	BOOL bNeedRestart = FALSE;
	DWORD dwErr = 0;
	BOOL bRet = TRUE;
	SC_HANDLE hMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hMgr)
	{
		BOOL bMulti = m_pBtnMultLog->IsChecked();
		if (bMulti)
		{
			TCHAR tszPath[MAX_PATH] = {0};
			UINT unPathLen = GetSystemDirectory(tszPath, MAX_PATH);
			if ( tszPath[unPathLen-1] != _T('\\'))
				tszPath[unPathLen] = _T('\\');
			StrCat(tszPath, _T("drivers\\"));
			StrCat(tszPath, MULTILOG_FILENAME);
			if ( INVALID_FILE_ATTRIBUTES == GetFileAttributes(tszPath))
			{//重新释放文件
				HMODULE hResHandle = NULL;
				HRSRC hResource = ::FindResource(hResHandle, MAKEINTRESOURCE(IDR_DRIVER), _T("SYS"));
				if (hResource)
				{
					DWORD imageSize = ::SizeofResource(hResHandle, hResource);
					if (imageSize)
					{
						const void* pResourceData = ::LockResource(::LoadResource(hResHandle, hResource));
						if (pResourceData)
						{
							HANDLE hFile = CreateFile(tszPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
							if ( INVALID_HANDLE_VALUE != hFile )
							{
								DWORD dwWritten = 0;
								WriteFile(hFile, pResourceData, imageSize, &dwWritten, NULL);
								CloseHandle(hFile);
							}
						}
					}
				}
			}
			SC_HANDLE hCS = CreateService(hMgr, MULTILOG_SERVICENAME, MULTILOG_DISPLAYNAME, 
				SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
				tszPath, NULL, NULL, NULL, NULL, NULL);
			if (hCS)
			{
				bRet = StartService(hCS, 0, NULL);
				CloseServiceHandle(hCS);
			}
		}
		else
		{
			if ( IDYES == MessageBox(_T("此操作需要重启, 是否继续?"), _T("提示"), MB_YESNO | MB_ICONWARNING) )
			{
				SC_HANDLE hSvc = OpenService(hMgr, MULTILOG_SERVICENAME, SERVICE_ALL_ACCESS); //得到服务名的句柄
				if (hSvc)
				{
					bRet = DeleteService(hSvc);
					if (bRet)
						bNeedRestart = TRUE;
					CloseServiceHandle(hSvc);
				}
			}
		}		
		CloseServiceHandle(hMgr);
	}
	if ( ! bRet)
	{
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
		MessageBox(tszMsg, NULL, NULL);
	}
	else if (bNeedRestart)
	{
		system("shutdown -r -t 0");
	}
	m_pBtnMultLog->SetCheck(this->CheckMultiLog());
}
