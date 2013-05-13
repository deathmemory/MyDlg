#pragma  once
#include <windows.h>
namespace KDB
{
	VOID KeyPress(BYTE VK);
	VOID KeyDown(BYTE VK);
	VOID KeyUp(BYTE VK);
};
#include "Keyborder.cpp"