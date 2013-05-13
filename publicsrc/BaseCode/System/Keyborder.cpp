#include "Keyborder.h"
namespace KDB
{
	VOID KeyPress(BYTE VK)
	{
		keybd_event(VK,MapVirtualKey(VK,0),0,0);
		Sleep(200);	
		keybd_event(VK,MapVirtualKey(VK,0),KEYEVENTF_KEYUP,0);
		Sleep(100);
	}
	VOID KeyDown(BYTE VK)
	{
		keybd_event(VK,MapVirtualKey(VK,0),0,0);
	}
	VOID KeyUp(BYTE VK)
	{
		keybd_event(VK,MapVirtualKey(VK,0),KEYEVENTF_KEYUP,0);
	}
};
