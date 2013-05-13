#include "StdAfx.h"
#include "PersonManager.h"


CPersonManager::CPersonManager(void)
{
	m_pperson = NULL;
}


CPersonManager::~CPersonManager(void)
{
}

BOOL CPersonManager::Init(DWORD dwAddr/* = 0*/)
{
	if ( CBaseHook::Init(dwAddr) )
	{
		m_pperson = (Person*)dwAddr;
		if (IsBadReadPtr(m_pperson, sizeof(Person)))
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
}