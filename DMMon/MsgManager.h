#pragma once
#include "basehook.h"
class CMsgManager :
	public CBaseHook
{
public:
	CMsgManager(void);
	virtual ~CMsgManager(void);

	virtual BOOL Init(DWORD dwAddr = TRUE);
	virtual VOID UnInit();

	static BOOL g_bWatch;
	static DWORD WINAPI MsgThread(LPVOID);
private:
	HANDLE m_hMsgTread;
};

