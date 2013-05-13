#ifdef _WINDOWS
    #include "stdafx.h"
#endif
#include "cRandom.h"

CRndNum::CRndNum()
{
	DWORD CrtTm;
	int i;
	LARGE_INTEGER li={0};
	QueryPerformanceCounter(&li);
	CrtTm = li.LowPart;
	DWORD aa=(DWORD)16807;
	RNum[1]=(DWORD)CrtTm;
	for(i=2; i<=250; i++) RNum[i]=RNum[i-1]*aa;	

	CrtN=(unsigned char)251;	
	PrNA=(unsigned char)1;
	PrNB=(unsigned char)148;
};

DWORD CRndNum::GetRN01()
{
	RNum[++CrtN]=RNum[++PrNA] ^ RNum[++PrNB];
	return (DWORD)(RNum[CrtN]/(double)4294967295.0); //[0, 1)
};

DWORD CRndNum::GetRndNum(double Lt, double Rt)	//Lt < Rt;
{
	RNum[++CrtN]=RNum[++PrNA] ^ RNum[++PrNB];
	return (DWORD)(Lt + (Rt - Lt)*(double)RNum[CrtN]/(double)4294967295.0);
};
DWORD CRndNum::GetRndDWORD(double Lt, double Rt)
{
	LARGE_INTEGER li={0};
	QueryPerformanceCounter(&li);
	return GetRndNum(Lt,Rt)+li.LowPart;
}