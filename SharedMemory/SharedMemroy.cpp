// SharedMemroy.cpp : implementation file
//

#include "stdafx.h"
#include "SharedMemroy.h"

// CSharedMemroy
#define MUTEX_NAME				_T("DEATHMEMORY MIR2 IPC Mutex")
#define FILEMAPPING_NAME		_T("DEATHMEMORY MIR2 IPC")

CSharedMemroy::CSharedMemroy()
{
	m_hMutex = nullptr;
	m_hFileMapping = nullptr;
	m_pSharedMemory = nullptr;
	m_bOpened = false;
	Open();
}

CSharedMemroy::~CSharedMemroy()
{
	Close();
}

BOOL CSharedMemroy::Open()
{
	m_hMutex = CreateMutex(nullptr, TRUE, MUTEX_NAME);
	if (m_hFileMapping)
		return FALSE;

	m_hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(SharedMemory), FILEMAPPING_NAME);
	if (m_hFileMapping)
	{
		m_pSharedMemory  = (SharedMemory *)MapViewOfFile(m_hFileMapping, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
		if (m_pSharedMemory)
		{
			m_bOpened = true;
			return TRUE;
		}
		CloseHandle(m_hFileMapping);
		m_hFileMapping = nullptr;
	}
	return FALSE;
}

void CSharedMemroy::Close()
{
	if (m_pSharedMemory)
	{
		UnmapViewOfFile(m_pSharedMemory);
		m_pSharedMemory = nullptr;
	}
	if (m_hFileMapping)
	{
		CloseHandle(m_hFileMapping);
		m_pSharedMemory = nullptr;
	}
	if (m_hMutex)
		CloseHandle(m_hMutex);

	m_hMutex = nullptr;
	m_bOpened = false;
}

BOOL CSharedMemroy::SetClientPath( LPCTSTR lpszPath )
{
	if (!m_bOpened)
		return FALSE;
	
	WaitForSingleObject(m_hMutex, INFINITE);
	_tcscpy(m_pSharedMemory->cClientPath, lpszPath);
	ReleaseMutex(m_hMutex);
	return TRUE;
}

LPCTSTR CSharedMemroy::GetClientPath()
{
	if (!m_bOpened)
		return nullptr;

	return m_pSharedMemory->cClientPath;
}

BOOL CSharedMemroy::SetMainWindow( HWND hWnd )
{
	if (!m_bOpened)
		return FALSE;

	WaitForSingleObject(m_hMutex, INFINITE);
	m_pSharedMemory->hMainWnd = hWnd;
	ReleaseMutex(m_hMutex);
	return TRUE;
}

HWND CSharedMemroy::GetMainWindow()
{
	if (!m_bOpened)
		return nullptr;

	return m_pSharedMemory->hMainWnd;
}
