// dmDebug.h : main header file for the dmDebug DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CdmDebugApp
// See dmDebug.cpp for the implementation of this class
//

class CdmDebugApp : public CWinApp
{
public:
	CdmDebugApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
