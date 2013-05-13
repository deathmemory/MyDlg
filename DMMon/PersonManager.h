#pragma once
#include "BaseHook.h"
#include "../structs/pepolestruct.h"

class CPersonManager : public CBaseHook
{
public:
	CPersonManager(void);
	virtual ~CPersonManager(void);
	virtual BOOL Init(DWORD dwAddr = 0);
private:
public:
	Person* m_pperson;
};

extern CPersonManager g_PersonManager;