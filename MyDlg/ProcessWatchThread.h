#pragma once



// CProcessWatchThread

class CProcessWatchThread : public CWinThread
{
	DECLARE_DYNCREATE(CProcessWatchThread)

protected:
	CProcessWatchThread();           // protected constructor used by dynamic creation
	virtual ~CProcessWatchThread();

public:
	static CProcessWatchThread * NewInstance();
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	void StartWatch();
	void CancelWatch();
	void ReSet();
	void SetNotifyWindow(HWND hWnd);
	CString strEXE;
	CString strDLL;
protected:
	HWND m_hNotifyWindow;
	bool m_bCancel;
	BOOL m_bReset;
	DECLARE_MESSAGE_MAP()
};


