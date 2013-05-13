// WaitForObjectsThread.cpp : implementation file
//

#include "stdafx.h"
#include "WaitForObjectsThread.h"

// CWaitForObjectsThread

IMPLEMENT_DYNCREATE(CWaitForObjectsThread, CWinThread)

CWaitForObjectsThread::CWaitForObjectsThread()
{
	m_dwLimit = 20;
	m_pHandles = new HANDLE[m_dwLimit];
	m_dwHandleCount = 0;
}

CWaitForObjectsThread::~CWaitForObjectsThread()
{
	if (m_pHandles)
		delete []m_pHandles;
}

BOOL CWaitForObjectsThread::InitInstance()
{
	WaitForMultipleObjects(m_dwHandleCount, m_pHandles, TRUE, INFINITE);
	return FALSE;
}

int CWaitForObjectsThread::ExitInstance()
{
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CWaitForObjectsThread, CWinThread)
END_MESSAGE_MAP()

void CWaitForObjectsThread::AddObject( HANDLE hObject )
{
	if (m_dwHandleCount == m_dwLimit)
	{
		m_dwLimit += 20;
		HANDLE *pTemp = new HANDLE[m_dwLimit];
		memcpy(pTemp, m_pHandles, sizeof(HANDLE) * m_dwHandleCount);
		delete []m_pHandles;
		m_pHandles = pTemp;
	}
	m_pHandles[m_dwHandleCount++] = hObject;
}

// CWaitForObjectsThread message handlers
