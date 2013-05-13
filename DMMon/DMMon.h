#pragma once

void InitHookApi(DWORD dw = 0);
void ExitHookApi();

DWORD HookItemFilter();
//DWORD __declspec(naked) ItemFilterProc();

DWORD HookRecvMsg();

VOID MyMessagePatch(HANDLE hInst);
VOID MyMessageDisPatch();
LRESULT CALLBACK MyWindowProc(          HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	);
