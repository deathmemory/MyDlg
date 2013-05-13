#pragma once


// CDlg_BaseSet dialog

class CDlg_BaseSet : public CDialogEx
{
	DECLARE_DYNAMIC(CDlg_BaseSet)

public:
	CDlg_BaseSet(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlg_BaseSet();

// Dialog Data
	enum { IDD = IDD_DIALOG_CHILD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void InitControls();
	virtual void Notify(TNotifyUI& msg);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
