#pragma once
class CRecvMsgMoniter
{
public:
	CRecvMsgMoniter(void);
	virtual ~CRecvMsgMoniter(void);
	DWORD HookRecvYellowMsg();
	VOID UnHookRecvYellowMsg();
	static void __stdcall RecvMsgFilter(CHAR* pszContent);
};

