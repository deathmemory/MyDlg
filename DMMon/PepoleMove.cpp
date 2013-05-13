#include "StdAfx.h"
#include "PepoleMove.h"
#include "PersonManager.h"

#include <math.h>

CPepoleMove::CPepoleMove(void)
{
	m_dwMoveAddr = 0;
}


CPepoleMove::~CPepoleMove(void)
{
}

VOID CPepoleMove::MoveTo( DWORD dwY, DWORD dwX )// 走到
{
	if ( ! g_PersonManager.IsInit() || ! this->IsInit())
		return;

	int py = g_PersonManager.m_pperson->pos.uY;
	int px = g_PersonManager.m_pperson->pos.uX;
	while (py != dwY || px != dwX)
	{
		MOVE_TYPE movetype = enm_run;
		if (abs(py - (int)dwY) >= 2 || abs(px - (int)dwX) >= 2)
			movetype = enm_run;
		else
			movetype = enm_step;
		this->Move(movetype, dwY, dwX);
		Sleep(100);
		py = g_PersonManager.m_pperson->pos.uY;
		px = g_PersonManager.m_pperson->pos.uX;
	}
	Sleep(1500);	// 等待拾取物品
}

VOID CPepoleMove::Move(MOVE_TYPE mType, DWORD dwY, DWORD dwX, MOVE_TOWARD toward)
{// move 完了之后 人物坐标需要手工赋值才能捡起东西
	if ( ! g_PersonManager.IsInit() || ! this->IsInit())
		return;

	DWORD dwAddr = CBaseHook::GetAddr();
	__asm
	{
		pushad
		pushfd
		mov ecx, dwX
		mov edx, mType
		push dwY
		push toward
		mov eax, dwAddr
		call eax
		popfd
		popad
	}

	g_PersonManager.m_pperson->pos.uX = dwX;
	g_PersonManager.m_pperson->pos.uY = dwY;
}