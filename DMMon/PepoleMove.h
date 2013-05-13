#pragma once
#include "BaseHook.h"

enum MOVE_TOWARD
{
	enm_move_up,
	enm_move_upright,
	enm_move_right,
	enm_move_rightdown,
	enm_move_down,
	enm_move_leftdown,
	enm_move_left,
	enm_move_leftup
};

enum MOVE_TYPE
{
	enm_step = 0xBC3,
	enm_run = 0xBC5
};
class CPepoleMove : public CBaseHook
{
public:
	CPepoleMove(void);
	virtual ~CPepoleMove(void);

	VOID Move(MOVE_TYPE mType, DWORD dwY, DWORD dwX, MOVE_TOWARD toward = enm_move_up);	// ×ß
	VOID MoveTo( DWORD dwY, DWORD dwX );// ×ßµ½
private:
	DWORD m_dwMoveAddr;
};

extern CPepoleMove g_PepoleMove;