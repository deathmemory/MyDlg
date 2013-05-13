#include "StdAfx.h"
#include "CallHero.h"


CCallHero::CCallHero(void)
{
}


CCallHero::~CCallHero(void)
{
}

VOID CCallHero::CallHero(HERO_PRO pro, HERO_CALL_STATE callState/* = enm_hero_call*/
	, HERO_CALL_CHARACTER character/* = enm_hero_character_assistant*/)
{
	DWORD dwAddr = this->GetAddr();
	if ( ! dwAddr )
		return ;
	DWORD dwECX = *(DWORD*)((*(DWORD*)*(DWORD*)*(DWORD*)(dwAddr+14))+4);
	DWORD dwEAX = *(DWORD*)*(DWORD*)*(DWORD*)(dwAddr+24);
	DWORD dwCallAddr = dwAddr+36;
	DWORD dwOffset = *(DWORD*)dwCallAddr;
	dwCallAddr = dwCallAddr + dwOffset + 4;
	__asm
	{
		/* 
		d [[[6AC1E0]]+4]	=> ecx

		push main/assistant character
		push idx
		push 0
		push 0
		push 0
		mov     eax, dword ptr [6AC1E0]		
		mov     eax, dword ptr [eax]
		mov     ecx, dword ptr [eax+4]
		mov     eax, dword ptr [6AC504]
		mov     eax, dword ptr [eax]
		mov     edx, 41A
		*/
		pushad
		push character
		push pro
		push 0
		push 0
		push 0
		mov ecx, dwECX
		mov eax, dwEAX
		mov edx, callState
		call dwCallAddr
		popad
	}
}