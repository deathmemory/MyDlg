#pragma once
#define _CRndNum
#include <windows.h>
//using namespace std;

class CRndNum	//Rnd250, initiated with Rnd16807;
{
//	static unsigned long int RNum[256];
//	static unsigned char CrtN, PrNA, PrNB;
	DWORD RNum[256];
	BYTE CrtN, PrNA, PrNB;

public:
	CRndNum();
	DWORD GetRN01(void);	//Return a between [0, 1);
	DWORD GetRndNum(double Lt, double Rt);
	DWORD GetRndDWORD(double Lt=0x111111, double Rt=0xFFFFFF);

};
