#pragma once
#include "basehook.h"

typedef enum Hero_Profession
{
	enm_hero_soldier,
	enm_hero_master,
	enm_hero_priest
} HERO_PRO, *LPHERO_PRO;

typedef enum Hero_call_state
{
	enm_hero_call = 0x41A,		//召唤
	enm_hero_callback			//收回
} HERO_CALL_STATE, *LPHERO_CALL_STATE;

typedef enum Hero_call_character
{
	enm_hero_character_main,		//主将
	enm_hero_character_assistant	//副将
} HERO_CALL_CHARACTER, *LPHERO_CALL_CHARACTER;

class CCallHero :
	public CBaseHook
{
public:
	CCallHero(void);
	virtual ~CCallHero(void);
	VOID CallHero(HERO_PRO pro, HERO_CALL_STATE callState = enm_hero_call
		, HERO_CALL_CHARACTER character = enm_hero_character_assistant);
};

extern CCallHero g_CallHero;