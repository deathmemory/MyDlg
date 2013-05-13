#pragma once

class CSharedMemroy 
{
public:
	CSharedMemroy();
	~CSharedMemroy();

	BOOL Open();
	void Close();

	BOOL SetMainWindow(HWND hWnd);
	HWND GetMainWindow();

	BOOL SetClientPath(LPCTSTR lpszPath);
	LPCTSTR GetClientPath();
	
protected:
	struct SharedMemory
	{
		HWND hMainWnd;
		TCHAR cClientPath[MAX_PATH];
	};

	HANDLE m_hMutex;
	HANDLE m_hFileMapping;
	SharedMemory *m_pSharedMemory;

	bool m_bOpened;
};


