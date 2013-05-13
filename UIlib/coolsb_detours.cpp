
#include "stdafx.h"
#include <windows.h>
#include "coolsb_detours.h"
#include "coolscroll.h"
#include "detours.h"

#pragma warning(disable:4100)   // Trampolines don't use formal parameters.

typedef BOOL (WINAPI *pfnDetour_EnableScrollBar)(HWND hwnd, UINT wSBflags, UINT wArrows);
pfnDetour_EnableScrollBar Detour_EnableScrollBar = EnableScrollBar;

typedef BOOL (WINAPI *pfnDetour_GetScrollInfo)	(HWND hwnd, int fnBar, LPSCROLLINFO lpsi);
pfnDetour_GetScrollInfo Detour_GetScrollInfo = GetScrollInfo;

typedef int  (WINAPI *pfnDetour_GetScrollPos)	(HWND hwnd, int nBar);
pfnDetour_GetScrollPos Detour_GetScrollPos = GetScrollPos;

typedef BOOL (WINAPI *pfnDetour_GetScrollRange)	(HWND hwnd, int nBar, LPINT lpMinPos, LPINT lpMaxPos);
pfnDetour_GetScrollRange Detour_GetScrollRange = GetScrollRange;

typedef int  (WINAPI *pfnDetour_SetScrollInfo)	(HWND hwnd, int fnBar, LPCSCROLLINFO lpsi, BOOL fRedraw);
pfnDetour_SetScrollInfo Detour_SetScrollInfo = SetScrollInfo;

typedef int  (WINAPI *pfnDetour_SetScrollPos)	(HWND hwnd, int nBar, int nPos, BOOL fRedraw);
pfnDetour_SetScrollPos Detour_SetScrollPos = SetScrollPos;

typedef int  (WINAPI *pfnDetour_SetScrollRange)	(HWND hwnd, int nBar, int nMinPos, int nMaxPos, BOOL fRedraw);
pfnDetour_SetScrollRange Detour_SetScrollRange = SetScrollRange;

typedef BOOL (WINAPI *pfnDetour_ShowScrollBar)	(HWND hwnd, int wBar, BOOL fShow);
pfnDetour_ShowScrollBar Detour_ShowScrollBar = ShowScrollBar;

static BOOL WINAPI Tramp_EnableScrollBar(HWND hwnd, int wSBflags, UINT wArrows)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_EnableScrollBar(hwnd, wSBflags, wArrows);
	else
		return Detour_EnableScrollBar(hwnd, wSBflags, wArrows);
}

static BOOL WINAPI Tramp_GetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_GetScrollInfo(hwnd, fnBar, lpsi);
	else
		return Detour_GetScrollInfo(hwnd, fnBar, lpsi);
}

static int	 WINAPI Tramp_GetScrollPos(HWND hwnd, int nBar)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_GetScrollPos(hwnd, nBar);
	else
		return Detour_GetScrollPos(hwnd, nBar);
}

static BOOL WINAPI Tramp_GetScrollRange(HWND hwnd, int nBar, LPINT lpMinPos, LPINT lpMaxPos)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_GetScrollRange(hwnd, nBar, lpMinPos, lpMaxPos);
	else
		return Detour_GetScrollRange(hwnd, nBar, lpMinPos, lpMaxPos);
}

static int	 WINAPI Tramp_SetScrollInfo(HWND hwnd, int fnBar, LPSCROLLINFO lpsi, BOOL fRedraw)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_SetScrollInfo(hwnd, fnBar, lpsi, fRedraw);
	else
		return Detour_SetScrollInfo(hwnd, fnBar, lpsi, fRedraw);
}

static int  WINAPI Tramp_SetScrollPos(HWND hwnd, int nBar, int nPos, BOOL fRedraw)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_SetScrollPos(hwnd, nBar, nPos, fRedraw);
	else
		return Detour_SetScrollPos(hwnd, nBar, nPos, fRedraw);
}

static int  WINAPI Tramp_SetScrollRange(HWND hwnd, int nBar, int nMinPos, int nMaxPos, BOOL fRedraw)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_SetScrollRange(hwnd, nBar, nMinPos, nMaxPos, fRedraw);
	else
		return Detour_SetScrollRange(hwnd, nBar, nMinPos, nMaxPos, fRedraw);
}

static BOOL WINAPI Tramp_ShowScrollBar		(HWND hwnd, int wBar, BOOL fShow)
{
	if(CoolSB_IsCoolScrollEnabled(hwnd))	
		return CoolSB_ShowScrollBar(hwnd, wBar, fShow);
	else
		return Detour_ShowScrollBar(hwnd, wBar, fShow);
}

BOOL WINAPI CoolSB_InitializeApp(void)
{

	DetourTransactionBegin();
	DetourUpdateThread( GetCurrentThread());

	if( DetourAttach( &(PVOID&)Detour_EnableScrollBar, Tramp_EnableScrollBar) != NO_ERROR)
	{
		return FALSE;
	}


	DetourAttach(&(PVOID&)Detour_EnableScrollBar, Tramp_EnableScrollBar);
	DetourAttach(&(PVOID&)Detour_GetScrollInfo,   Tramp_GetScrollInfo);
	DetourAttach(&(PVOID&)Detour_GetScrollPos,    Tramp_GetScrollPos);
	DetourAttach(&(PVOID&)Detour_GetScrollRange,  Tramp_GetScrollRange);
	DetourAttach(&(PVOID&)Detour_SetScrollInfo,   Tramp_SetScrollInfo);
	DetourAttach(&(PVOID&)Detour_SetScrollPos,    Tramp_SetScrollPos);
	DetourAttach(&(PVOID&)Detour_SetScrollRange,  Tramp_SetScrollRange);
	DetourAttach(&(PVOID&)Detour_ShowScrollBar,   Tramp_ShowScrollBar);

		// don't actually use this feature within coolsb yet, but we might need it
	CoolSB_SetESBProc(Detour_EnableScrollBar);



	if( DetourTransactionCommit() != NO_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL WINAPI CoolSB_UninitializeApp(void)
{

	DetourTransactionBegin();
	DetourUpdateThread( GetCurrentThread());

	if( DetourAttach( &(PVOID&)Detour_EnableScrollBar, Tramp_EnableScrollBar) != NO_ERROR)
	{
		return FALSE;
	}


	DetourDetach(&(PVOID&)Detour_EnableScrollBar, Tramp_EnableScrollBar);
	DetourDetach(&(PVOID&)Detour_GetScrollInfo,   Tramp_GetScrollInfo);
	DetourDetach(&(PVOID&)Detour_GetScrollPos,    Tramp_GetScrollPos);
	DetourDetach(&(PVOID&)Detour_GetScrollRange,  Tramp_GetScrollRange);
	DetourDetach(&(PVOID&)Detour_SetScrollInfo,   Tramp_SetScrollInfo);
	DetourDetach(&(PVOID&)Detour_SetScrollPos,    Tramp_SetScrollPos);
	DetourDetach(&(PVOID&)Detour_SetScrollRange,  Tramp_SetScrollRange);
	DetourDetach(&(PVOID&)Detour_ShowScrollBar,   Tramp_ShowScrollBar);

	// don't actually use this feature within coolsb yet, but we might need it
	CoolSB_SetESBProc(Detour_EnableScrollBar);



	if( DetourTransactionCommit() != NO_ERROR)
	{
		return FALSE;
	}

	return TRUE;
}
