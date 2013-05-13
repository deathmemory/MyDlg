#pragma once
#include "basehook.h"
enum MSGCOLOR
{//ÑÕÉ«	[ABGR]
	mclr_black	= 0x00000000,
	mclr_yellow	= 0x0000e7f7,
	mclr_red	= 0x000000ff,
	mclr_green	= 0x0000ff00,
	mclr_blue	= 0x00ff0000,
	mclr_white	= 0x00ffffff
};

typedef struct _tagContextDef
{
	DWORD dwFlag;
	DWORD dwLen;
	CHAR szContext[MAX_PATH];
} CONTEXTDEF, LPCONTEXTDEF;

typedef struct  _tagMsgStruct
{
	MSGCOLOR backclr;
	MSGCOLOR fontclr;
	CONTEXTDEF contextdef;
}MSGSTRUCT, *LPMSGSTRUCT;

class CRecvMsgManager :
	public CBaseHook
{
public:
	CRecvMsgManager(void);
	virtual ~CRecvMsgManager(void);

	virtual BOOL Init(DWORD dwAddr = 0);
	virtual VOID UnInit();
	BOOL GetAddMessage();
	VOID AddMessage(LPMSGSTRUCT lpmsg);
	VOID AddMessage(CHAR* lpContext, MSGCOLOR fontclr = mclr_green, MSGCOLOR backclr = mclr_black);
private:
	DWORD m_dwMsgOffset;
	DWORD m_dwAddMsgAddr;
	DWORD m_dwAddMsgEAX;
};

extern CRecvMsgManager g_RecvMsgManager;

