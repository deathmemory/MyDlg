#include "StdAfx.h"
#include "RecvMsgMoniter.h"
#include "../publicsrc/BaseCode/hook/inline/HookInternal.h"

static HOOK_INTERNAL g_MsgFilter;

CRecvMsgMoniter::CRecvMsgMoniter(void)
{
}


CRecvMsgMoniter::~CRecvMsgMoniter(void)
{
}


void CRecvMsgMoniter::RecvMsgFilter(CHAR* pszContent)
{
	//MessageBox(0, 0, 0, 0);
	CHAR* pMsg = (CHAR*) pszContent;
	if (memcmp(pszContent, "(!)", 3) != 0)
	{
		MessageBoxA(0, pszContent, pszContent, 0);
	}
}

DWORD __declspec(naked) RecvMsgProc()
{
	__asm
	{
		pushad
		pushfd
		push ecx
		call CRecvMsgMoniter::RecvMsgFilter
		popfd
		popad
		lea eax, dword ptr [g_MsgFilter.RestoreCodeMem.JmpBack];
		jmp eax
	}
}
DWORD CRecvMsgMoniter::HookRecvYellowMsg()
{
	/*
	d ecx		// º°»°ÄÚÈÝ
	00629880    55              push    ebp							;yellow , red
	00629881    8BEC            mov     ebp, esp
	00629883    6A 00           push    0
	00629885    6A 00           push    0
	00629887    53              push    ebx
	00629888    56              push    esi
	00629889    8BF1            mov     esi, ecx
	0062988B    8BDA            mov     ebx, edx
	0062988D    33C0            xor     eax, eax
	0062988F    55              push    ebp
	00629890    68 7A996200     push    0062997A
	00629895    64:FF30         push    dword ptr fs:[eax]
	00629898    64:8920         mov     dword ptr fs:[eax], esp
	0062989B    A1 A8066A00     mov     eax, dword ptr [6A06A8]
	006298A0    8038 00         cmp     byte ptr [eax], 0
	006298A3    0F84 B6000000   je      0062995F
	006298A9    8D45 FC         lea     eax, dword ptr [ebp-4]
	006298AC    8BD6            mov     edx, esi
	006298AE    E8 3DB0DDFF     call    004048F0

	558B??6A??6A??53568B??8B??33??5568????????64????64????A1????????80????0F??????????8D????8B??E8
	558B??6A??6A??53568B??8B??33??5568????????64????64????A1????????80
	*/
	CHAR szSearchCode[0x43] = {0};
	__asm
	{
		MOV DWORD PTR [szSearchCode+00000h],'B855'
		MOV DWORD PTR [szSearchCode+00004h],'A6**'
		MOV DWORD PTR [szSearchCode+00008h],'A6**'
		MOV DWORD PTR [szSearchCode+0000Ch],'35**'
		MOV DWORD PTR [szSearchCode+00010h],'B865'
		MOV DWORD PTR [szSearchCode+00014h],'B8**'
		MOV DWORD PTR [szSearchCode+00018h],'33**'
		MOV DWORD PTR [szSearchCode+0001Ch],'55**'
		MOV DWORD PTR [szSearchCode+00020h],'**86'
		MOV DWORD PTR [szSearchCode+00024h],'****'
		MOV DWORD PTR [szSearchCode+00028h],'46**'
		MOV DWORD PTR [szSearchCode+0002Ch],'****'
		MOV DWORD PTR [szSearchCode+00030h],'**46'
		MOV DWORD PTR [szSearchCode+00034h],'1A**'
		MOV DWORD PTR [szSearchCode+00038h],'****'
		MOV DWORD PTR [szSearchCode+0003Ch],'****'
		MOV WORD PTR [szSearchCode+00040h],'08'
		MOV BYTE PTR [szSearchCode+00042h],0
	}
	DWORD pdwCif = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),(CHAR*)szSearchCode,SEARCH_PAGE_EXEC);
	if (pdwCif)
	{
		return g_MsgFilter.InitHookCurrentProcess( 0, (VOID*)pdwCif, (VOID*)&RecvMsgProc, TRUE );
	}
	return -1;
}

VOID CRecvMsgMoniter::UnHookRecvYellowMsg()
{
	g_MsgFilter.UnhookAddressCurrentProcess();
}