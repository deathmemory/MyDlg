#pragma once
#include "basehook.h"

#define ITEM_STR_MAX				0x10*8

class CAutoCommit :
	public CBaseHook
{
public:
	CAutoCommit(void);
	virtual ~CAutoCommit(void);
	
	virtual VOID UnInit();

	BOOL InitRecover();
	VOID UnInitRecover();
	BOOL IsRecoverInit();
	VOID GetCurItem();
	VOID RecoverTest();
private:
	DWORD m_dwRecoverAddr;
	DWORD m_dwItemStrAddr;
	DWORD m_dwCurItemAddr;
	BOOL m_bRecoverInit;
	CHAR m_szCurItemTag[ITEM_STR_MAX];
};

extern CAutoCommit g_AutoCommit;