// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "DMMon.h"
#include "../publicsrc/BaseCode/BaseCode.hpp"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		InitHookApi((DWORD)hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		ExitHookApi();
		break;
	}
	return TRUE;
}

