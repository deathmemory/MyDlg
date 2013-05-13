#pragma once

typedef struct tagItemStruct 
{
	BYTE cNameLen;		//名字长度
	BYTE szName[15];	//物品名
	WORD wID;			//物品ID
	WORD wUnknow1;
	WORD wUnknow2;
	WORD wWeight;		//重量
	WORD wUnknow3;
	WORD wUnknow4;
	WORD wUnknow5;
	WORD wNeedThing;	//需要的条件
	WORD wlucky;		//幸运
	WORD wVeracity;		//准确
	WORD wUnknow6;
	WORD wUnknow7;
	WORD wAttackMin;	//攻击下限
	WORD wAttackMax;	//攻击上限
	WORD wMagicMin;		//魔法下限
	WORD wMagicMax;		//魔法上限
	WORD wSCMin;		//道术下限
	WORD wSCMax;		//道术上限
	WORD wUnknow8;
	WORD wUnknow9;
	WORD wUnknow10;
	WORD wUnknow11;
	WORD wDurabilityMin;	//持久下限
	WORD wDurabilityMax;	//持久上限
	WORD wUnknow12;
	WORD wUnknow13;
	WORD wUnknow14;
	WORD wUnknow15;
	WORD wUnknow16;
	WORD wUnknow17;
	WORD wUnknow18;
	WORD wUnknow19;
	WORD wUnknow20;
	WORD wUnknow21;
	WORD wUnknow22;
	WORD wUnknow23;
	WORD wUnknow24;
	WORD wUnknow25;
	WORD wUnknow26;
	WORD wUnknow27;
} ITEMINFO, *LPITEMINFO;