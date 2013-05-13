// Dlg_BaseSet.cpp : implementation file
//

#include "stdafx.h"
#include "MyDlg.h"
#include "Dlg_BaseSet.h"
#include "afxdialogex.h"


// CDlg_BaseSet dialog

IMPLEMENT_DYNAMIC(CDlg_BaseSet, CDialogEx)

CDlg_BaseSet::CDlg_BaseSet(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDlg_BaseSet::IDD, pParent)
{

}

CDlg_BaseSet::~CDlg_BaseSet()
{
}

void CDlg_BaseSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlg_BaseSet, CDialogEx)
END_MESSAGE_MAP()


// CDlg_BaseSet message handlers

void CDlg_BaseSet::InitControls()
{

}

void CDlg_BaseSet::Notify(TNotifyUI& msg)
{

}

BOOL CDlg_BaseSet::OnInitDialog()
{
	InitControls();

	return TRUE;
}