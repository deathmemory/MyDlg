#pragma once



// CWaitForObjectsThread

class CWaitForObjectsThread : public CWinThread
{
	DECLARE_DYNCREATE(CWaitForObjectsThread)

protected:
	CWaitForObjectsThread();           // protected constructor used by dynamic creation
	virtual ~CWaitForObjectsThread();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	void AddObject(HANDLE hObject);

protected:
	DECLARE_MESSAGE_MAP()

private:
	HANDLE * m_pHandles;
	DWORD m_dwHandleCount;
	DWORD m_dwLimit;
};
