
// MyDlgDlg.h : header file
//

#pragma once
#include "./minitray/DMTrayIcon.h"
#include "ProcessWatchThread.h"

#define DLG_MAIN_WIDTH				800
#define DLG_MAIN_HEIGHT				600

class CProcessWatchThread;
// CMyDlgDlg dialog
class CMyDlgDlg : public CSkinDialog
{
	enum DM_WIDGET_ID
	{
		ID_LAB_TEST = 1000,
		ID_LAB_RECORD,

		ID_BTN_MIN,
		ID_BTN_CLOSE,
		ID_BTN_SET,
		ID_BTN_START,
		ID_BTN_MULTILOG,

		ID_CHK_LOGMUSIC,
		ID_CHK_AD,
		ID_CHK_,


		ID_DM_END
	};
	enum DM_TIMER_ID
	{
		TID_RECORD = WM_USER + 500
	};
// Construction
public:
	CMyDlgDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MYDLG_DIALOG };
public:
	CString GetLocalDir_();
	void AddNotDragArea(CRect rc);
	BOOL IsNotDragArea(POINT point);
	BOOL SetDlgSkinFromFile(LPTSTR lpFileName);
	CString GetGamePath();	
	void GetGameParamTimer();	// 获取游戏运行参数
	void AppSetBeforeStartGame();	// 游戏运行先应用各种配置
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void InitControls();
	virtual void Notify(TNotifyUI& msg);

	void Exit();

	void OnBtnClickMin();
	void OnBtnClickClose();
	void OnBtnClickStartGame();
	void OnBtnClickSetQuickStart();
	void OnBtnClickMultiLog();
// Implementation
protected:
	HICON m_hIcon;
	BOOL CheckMultiLog();
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CDMTrayIcon m_dmTrayIcon;
	CMenu m_mTrayMenu;
private:
	CString m_strLocalDir_;
	CString m_strGameDir_;
	CRect m_ClientRect;
	vector<CRect> m_rcNotDragArea;
	CUIButton* m_pBtnChkAD;
	CUIButton* m_pBtnChkLogMusic;
	CUIButton* m_pBtnMultLog;	//多开
	CProcessWatchThread* m_pWatch_Mir1_dat_Thread;
public:
	afx_msg LRESULT OnNcHitTest(CPoint point);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnMnuStartgame();
	afx_msg void OnMnuExit();
};
