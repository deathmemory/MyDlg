#pragma once


// CDMDbg dialog

class __declspec( dllexport ) CDMDbg : public CDialogEx
{
	DECLARE_DYNAMIC(CDMDbg)

public:
	CDMDbg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDMDbg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	DWORD m_dwEAX;
	DWORD m_dwECX;
	DWORD m_dwEDX;
	DWORD m_dwEBX;
	DWORD m_dwCallAddr;
	CString m_strEAX;
	CString m_strECX;
	CString m_strEDX;
	CString m_strEBX;
	afx_msg void OnBnClickedButton1();
	DWORD m_dwParam1;
	DWORD m_dwParam2;
	DWORD m_dwParam3;
	DWORD m_dwParam4;
	CString m_strParam1;
	CString m_strParam2;
	CString m_strParam3;
	CString m_strParam4;
};
