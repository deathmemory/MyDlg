#pragma once
class CBaseHook
{
public:
	CBaseHook(void){m_bInit = FALSE; m_dwAddr = 0;}
	virtual ~CBaseHook(void){}
	virtual BOOL Init(DWORD dwAddr = 0)	{if (dwAddr){m_dwAddr = dwAddr;	m_bInit = TRUE;	return TRUE;}return FALSE;}
	virtual VOID UnInit(){};

	BOOL IsInit(){return m_bInit;}
	DWORD GetAddr(){return m_dwAddr;}
private:
	BOOL m_bInit;
	DWORD m_dwAddr;
};

