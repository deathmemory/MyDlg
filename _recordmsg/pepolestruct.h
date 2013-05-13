#pragma once

struct PersonPos
{
	USHORT uX;
	USHORT uY;
};
struct Person
{
	DWORD* pdwPtr;			// 回调地址？
	DWORD dwUnknow4;
	PersonPos pos;			// 人物坐标 {}
	DWORD dwUnknow5;		// 03000100 "PE"	-4
	DWORD dwUnknow6_0[8];	//					-12
	DWORD dwUnknow6_0_1;
	DWORD* pdwPlayerName;	//
	DWORD dwUnknow6_1;	// 
	DWORD dwDefense_U;		// 防御上限		-16
	DWORD dwMagicToken_L;	// 魔法防御下限
	DWORD dwQuick;			// 敏捷
	DWORD dwAttack_L;		// 攻击下限
	DWORD dwAttack_U;		// 攻击上限		-20
	DWORD dwBrawn_U;		// 腕力上限
	DWORD dwHP1;
	DWORD dwMagic_L;		// 魔法下限
	DWORD dwSC_L;			// 道术下限 ???	-24
	DWORD dwMagic_U;		// 魔法上限
	DWORD dwUnknow8;
	DWORD dwSC_U;			// 道术上限
	DWORD dwMP1;			//				-28
	USHORT uUnknow_4;
	USHORT uAccuracy;		// 精确度
	DWORD dwMagicToken_U;	// 魔法防御上限
	DWORD dwUnknow11;
	DWORD dwMP2;
	DWORD dwCurrentXP;		// current xp
	DWORD dwLevelupXP;		// level up xp
	DWORD dwPackSack_L;		// 背包重量下限
	DWORD dwPackSack_U;		// 背包重量上限
	DWORD dwUnknow13;
	DWORD dwHP2;
	DWORD dwDress_L;		// 穿着重量下限
	DWORD dwDress_U;		// 穿着重量上限
	DWORD dwBrawn_L;		// 腕力下限
	DWORD dwDefense_L;		// 防御下限

	DWORD dwLevel;


};