// DMDbg.cpp : implementation file
//

#include "stdafx.h"
#include "dmDebug.h"
#include "DMDbg.h"
#include "afxdialogex.h"


// CDMDbg dialog

IMPLEMENT_DYNAMIC(CDMDbg, CDialogEx)

CDMDbg::CDMDbg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDMDbg::IDD, pParent)
	, m_dwEAX(0)
	, m_dwECX(0)
	, m_dwEDX(0)
	, m_dwEBX(0)
	, m_dwCallAddr(0)
	, m_strEAX(_T(""))
	, m_strECX(_T(""))
	, m_strEDX(_T(""))
	, m_strEBX(_T(""))
	, m_dwParam1(0)
	, m_dwParam2(0)
	, m_dwParam3(0)
	, m_dwParam4(0)
	, m_strParam1(_T(""))
	, m_strParam2(_T(""))
	, m_strParam3(_T(""))
	, m_strParam4(_T(""))
{

}

CDMDbg::~CDMDbg()
{
}

void CDMDbg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_dwEAX);
	DDX_Text(pDX, IDC_EDIT3, m_dwECX);
	DDX_Text(pDX, IDC_EDIT5, m_dwEDX);
	DDX_Text(pDX, IDC_EDIT7, m_dwEBX);
	DDX_Text(pDX, IDC_EDIT9, m_dwCallAddr);
	DDX_Text(pDX, IDC_EDIT2, m_strEAX);
	DDX_Text(pDX, IDC_EDIT4, m_strECX);
	DDX_Text(pDX, IDC_EDIT6, m_strEDX);
	DDX_Text(pDX, IDC_EDIT8, m_strEBX);
	DDX_Text(pDX, IDC_EDIT12, m_dwParam1);
	DDX_Text(pDX, IDC_EDIT13, m_dwParam2);
	DDX_Text(pDX, IDC_EDIT14, m_dwParam3);
	DDX_Text(pDX, IDC_EDIT15, m_dwParam4);
	DDX_Text(pDX, IDC_EDIT16, m_strParam1);
	DDX_Text(pDX, IDC_EDIT17, m_strParam2);
	DDX_Text(pDX, IDC_EDIT18, m_strParam3);
	DDX_Text(pDX, IDC_EDIT19, m_strParam4);
}


BEGIN_MESSAGE_MAP(CDMDbg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CDMDbg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CDMDbg message handlers


void CDMDbg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	if (m_strEAX)
		m_dwEAX = DWORD(m_strEAX.GetBuffer(m_strEAX.GetLength()));
	if (m_strECX)
		m_dwECX = DWORD(m_strECX.GetBuffer(m_strECX.GetLength()));
	if (m_strEDX)
		m_dwEDX = DWORD(m_strEDX.GetBuffer(m_strEDX.GetLength()));
	if (m_strEBX)
		m_dwEBX = DWORD(m_strEBX.GetBuffer(m_strEBX.GetLength()));

	if (m_strParam1)
		m_dwParam1 = DWORD(m_strParam1.GetBuffer(m_strParam1.GetLength()));
	if (m_strParam2)
		m_dwParam2 = DWORD(m_strParam2.GetBuffer(m_strParam2.GetLength()));
	if (m_strParam3)
		m_dwParam3 = DWORD(m_strParam3.GetBuffer(m_strParam3.GetLength()));
	if (m_strParam4)
		m_dwParam4 = DWORD(m_strParam4.GetBuffer(m_strParam4.GetLength()));

	if ( m_dwParam4 )
		__asm push m_dwParam4;
	if ( m_dwParam3 )
		__asm push m_dwParam3;
	if ( m_dwParam2 )
		__asm push m_dwParam2;
	if ( m_dwParam1 )
		__asm push m_dwParam1;
	__asm call MessageBoxA;

}
