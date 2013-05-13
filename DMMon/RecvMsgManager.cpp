#include "StdAfx.h"
#include "RecvMsgManager.h"
#include "../publicsrc/BaseCode/hook/inline/HookInternal.h"

HOOK_INTERNAL g_rcvInternal;
static DWORD g_dwEAX = 0;

CRecvMsgManager::CRecvMsgManager(void)
{
	m_dwMsgOffset = 0;
	m_dwAddMsgAddr = 0;
	m_dwAddMsgEAX = 0;
}


CRecvMsgManager::~CRecvMsgManager(void)
{
}

VOID __stdcall MsgFilter2(CHAR* lpszContext, DWORD dwFontColor, DWORD dwBackColor)
{
#ifdef _DEBUG
 	CHAR szstr[MAX_PATH*2] = {0};
 	sprintf(szstr, "bclr:0x%08x, fclr:0x%08x, eax:0x%08x, context: %s\r\n", dwBackColor, dwFontColor, g_dwEAX, lpszContext);
 	OutputDebugStringA(szstr);
#endif
}
VOID __declspec(naked) MsgFilter(DWORD dwBackColor)
{
	/*
	PARAM => 字体背景颜色
	EAX => 取值参照回显CALL外层
	ECX => 字体颜色	[AGBR]
	EDX => 内容[ALL]
	CHAR szContext[] = {"\x01\x00\x00\x00\x0E\x00\x00\x00this is a test\0"};
						标志位				发送回显示文字的长度
	*/
	__asm
	{
		push ebp
		mov ebp, esp
		sub esp, 200h
		pushad
		mov g_dwEAX, eax
		push dwBackColor
		push ecx
		push edx
		call MsgFilter2
		popad
		push dwBackColor
		lea ebx, g_rcvInternal.RestoreCodeMem.JmpBack
		call ebx
		leave; //mov esp, ebp; pop ebp
		ret 4
	}
}

BOOL CRecvMsgManager::Init(DWORD dwAddr/* = 0*/)
{
	BOOL bRet = CBaseHook::Init(dwAddr);
	if (bRet)
	{
		g_rcvInternal.InitHookCurrentProcess( 0, (VOID*)(CBaseHook::GetAddr()), (VOID*)&MsgFilter, TRUE );
	}
	return bRet;
}

VOID CRecvMsgManager::UnInit()
{
	g_rcvInternal.UnhookAddressCurrentProcess();
}

VOID CRecvMsgManager::AddMessage(CHAR* lpContext, MSGCOLOR fontclr, MSGCOLOR backclr)
{
	MSGSTRUCT msg;
	msg.backclr = backclr;
	msg.fontclr = fontclr;
	msg.contextdef.dwFlag = 0x01;	//flag
	msg.contextdef.dwLen = lstrlenA(lpContext);
	if(MAX_PATH-1 < msg.contextdef.dwLen)
		msg.contextdef.dwLen = MAX_PATH-1;
	memcpy(msg.contextdef.szContext, lpContext, msg.contextdef.dwLen);
	this->AddMessage(&msg);
}

VOID CRecvMsgManager::AddMessage(LPMSGSTRUCT lpmsg)
{
	if ( ! m_dwAddMsgAddr || ! m_dwAddMsgEAX )
		GetAddMessage();
	if ( ! m_dwAddMsgAddr || ! m_dwAddMsgEAX )
		return;

	/*
	PARAM => 字体背景颜色
	EAX => 取值参照回显CALL外层
	ECX => 字体颜色	[AGBR]
	EDX => 内容[ALL]
	CHAR szContext[] = {"\x01\x00\x00\x00\x0E\x00\x00\x00this is a test\0"};
						标志位				发送回显示文字的长度
	*/
	DWORD dwAddMsgAddr = m_dwAddMsgAddr;
	DWORD dwAddMsgEAX = m_dwAddMsgEAX;
	DWORD dwfontclr = lpmsg->fontclr;
	DWORD dwbackclr = lpmsg->backclr;
	LPVOID lpContext = LPVOID(lpmsg->contextdef.szContext);
	__asm
	{
		pushad
		mov eax, dwAddMsgEAX
		mov ecx, dwfontclr
		mov edx, lpContext
		push dwbackclr
		mov ebx, dwAddMsgAddr
		call ebx
		popad
	}
}

BOOL CRecvMsgManager::GetAddMessage()
{
	/*
	006251C6    68 0000FF00     push    0FF0000
	006251CB    A1 0CDB6A00     mov     eax, dword ptr [6ADB0C]
	006251D0    8B00            mov     eax, dword ptr [eax]
	006251D2    B9 FFFFFF00     mov     ecx, 0FFFFFF
	006251D7    8B55 FC         mov     edx, dword ptr [ebp-4]
	006251DA    E8 0DADFBFF     call    005DFEEC                         ; 自已回显Call
	006251DF    8973 24         mov     dword ptr [ebx+24], esi
	006251E2    33C0            xor     eax, eax
	006251E4    5A              pop     edx
	006251E5    59              pop     ecx
	006251E6    59              pop     ecx

	68????????A1????????8B??B9????????8B????E8????????89????33??5A5959
	*/
	CHAR szSearchCode[] = {"68********A1********8B**B9********8B****E8********89****33**5A5959\0"};
	DWORD dwcif = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(), szSearchCode, SEARCH_PAGE_EXEC);
	if (dwcif)
	{
		DWORD dwtmp = *(DWORD*)(dwcif + 6);
		m_dwAddMsgEAX = *(DWORD*)*(DWORD*)dwtmp;		//mov     eax, dword ptr [6ADB0C]
		dwtmp = dwcif + strlen("68********A1********8B**B9********8B****E8") / 2;
		m_dwAddMsgAddr = dwtmp + *(DWORD*)dwtmp + 4;	//call    005DFEEC     ; 自已回显Call
		return TRUE;
	}
	return FALSE;
}