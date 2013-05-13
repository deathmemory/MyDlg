
// MyDlg.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "../SharedMemory/SharedMemroy.h"

// CMyDlgApp:
// See MyDlg.cpp for the implementation of this class
//

class CMyDlgApp : public CWinApp
{
public:
	CMyDlgApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
// Implementation
	CSharedMemroy m_ShareMemory;
	DECLARE_MESSAGE_MAP()
};

extern CMyDlgApp theApp;