#include "StdAfx.h"
#include "SearchCodeManager.h"
#include "../publicsrc/BaseCode/hook/inline/HookInternal.h"

CSearchCodeManager g_SCManagerEx;
CMapItemsMoniter g_MapItemMoniter;
CPepoleMove g_PepoleMove;
CPersonManager g_PersonManager;
//CMsgManager g_MsgManager;
CCallHero g_CallHero;
CAutoCommit g_AutoCommit;
CRecvMsgManager g_RecvMsgManager;

CSearchCodeManager::CSearchCodeManager(void)
{
	m_dwpMoveAddr = 0;
	m_dwMapItemsAddr = 0;
	m_dwPersonStructAddr = 0;
	m_dwCallHeroAddr = 0;
	m_dwRecvMsgAddr = 0;
}


CSearchCodeManager::~CSearchCodeManager(void)
{
}

BOOL CSearchCodeManager::Init(DWORD dwAddr /*= TRUE*/)
{
	if (CBaseHook::Init(dwAddr))
	{
		// 	if ( -1 == g_recvMsgMoniter.HookRecvYellowMsg() )
		// 	{
		// 		// failed
		// 	}
		if (! this->SearchCodes() )
		{
			MessageBoxA(NULL, "sc failed !", NULL, NULL);
			return FALSE;
		}
 		if ( FALSE == g_MapItemMoniter.Init(this->GetMapItemsAddr()))
 		{
 			MessageBoxA(0, "g_MapItemMoniter init", 0, 0);
 		}
		if (FALSE == g_PersonManager.Init(this->GetPeronStructAddr()))
		{
			MessageBoxA(0, "g_PersonManager init", 0, 0);
		}
		if (FALSE == g_PepoleMove.Init(this->GetMoveAddr()))
		{
			MessageBoxA(0, "g_PepoleMove init", 0, 0);
		}
		if (FALSE == g_CallHero.Init(this->GetCallHeroAddr()))
		{
			MessageBoxA(0, "g_CallHero init", 0, 0);
		}
// 		if (FALSE == g_AutoCommit.Init(this->GetAutoCommitAddr()))
// 		{
// 			MessageBoxA(0, "g_AutoCommit init", 0, 0);
// 		}
// 		if (FALSE == g_RecvMsgManager.Init(this->GetRecvMsgAddr()))		//µ¥¶ÀµÄ
// 		{
// 			MessageBoxA(0, "g_RecvMsgManager init", 0, 0);
// 		}
		return TRUE;
	}
	return FALSE;
}

VOID CSearchCodeManager::UnInit()
{
	//g_recvMsgMoniter.UnHookRecvYellowMsg();
	if (g_MapItemMoniter.IsInit())	g_MapItemMoniter.UnInit();
	if (g_PepoleMove.IsInit())		g_PepoleMove.UnInit();
	if (g_PersonManager.IsInit())	g_PersonManager.UnInit();
	if (g_CallHero.IsInit())		g_CallHero.UnInit();
	if (g_RecvMsgManager.IsInit())	g_RecvMsgManager.UnInit();
}


BOOL CSearchCodeManager::SearchCodes()
{
	BOOL bRet = TRUE;	// bRet Óë¸÷º¯Êý·µ»ØÖµ½øÐÐ¡®Óë¡¯ÔËËã£¬Ö»ÓÐ¶¼ÎªTrueÊ±²ÅÄÜ·µ»ØTrue
	BOOL bTmpRet = FALSE;
	bTmpRet = this->SearchMoveAddr();
	if (FALSE == bTmpRet)
	{ // failed output
		MessageBoxA(NULL, "sc moveaddr !", NULL, NULL);
	}
	bRet &= bTmpRet;

	bTmpRet = this->SearchMapItemsAddr();
	if (FALSE == bTmpRet)
	{ // failed output
		MessageBoxA(NULL, "sc mpiaddr !", NULL, NULL);
	}
	bRet &= bTmpRet;

	bTmpRet = this->SearchPeronStructAddr();
	if (FALSE == bTmpRet)
	{ // failed output
		MessageBoxA(NULL, "sc per str !", NULL, NULL);
	}
	bRet &= bTmpRet;

	bTmpRet = this->SearchCallHeroAddr();
	if (FALSE == bTmpRet)
	{ // failed output
		MessageBoxA(NULL, "sc chaddr !", NULL, NULL);
	}
	bRet &= bTmpRet;

	bTmpRet = this->SearchRecvMsgAddr();
	if (FALSE == bTmpRet)
	{ // failed output
		MessageBoxA(NULL, "sc rmsg !", NULL, NULL);
	}
	bRet &= bTmpRet;

	return bRet;
}

BOOL CSearchCodeManager::SearchMoveAddr()
{
	/*

	ECX 0000001B	// X
	EDX 00000BC3	// ×ß¿©Ö¸Áî
	DWORD __stdcall Move(DWORD toward, DWORD Y);

	005C55B8   /EB 06           jmp     short 005C55C0		v 2.1.1.226 ×·¼ÓÍêÉÆ
	005BC102    C703 D20B0000   mov     dword ptr [ebx], 0BD2
	005BC108    8B43 08         mov     eax, dword ptr [ebx+8]
	005BC10B    50              push    eax
	005BC10C    8B43 0C         mov     eax, dword ptr [ebx+C]
	005BC10F    50              push    eax
	005BC110    8B4B 04         mov     ecx, dword ptr [ebx+4]
	005BC113    8B13            mov     edx, dword ptr [ebx]
	005BC115    8BC6            mov     eax, esi
	005BC117    E8 40350000     call    005BF65C                         ; top 4 mov real call addr

	EB??C7??????????8B????508B????508B????8B??8B??E8
	
	*/
	CHAR szSearchCode[0x31] = {0};
	__asm
	{
		MOV DWORD PTR [szSearchCode+00000h],'**BE'
		MOV DWORD PTR [szSearchCode+00004h],'**7C'
		MOV DWORD PTR [szSearchCode+00008h],'****'
		MOV DWORD PTR [szSearchCode+0000Ch],'****'
		MOV DWORD PTR [szSearchCode+00010h],'**B8'
		MOV DWORD PTR [szSearchCode+00014h],'05**'
		MOV DWORD PTR [szSearchCode+00018h],'**B8'
		MOV DWORD PTR [szSearchCode+0001Ch],'05**'
		MOV DWORD PTR [szSearchCode+00020h],'**B8'
		MOV DWORD PTR [szSearchCode+00024h],'B8**'
		MOV DWORD PTR [szSearchCode+00028h],'B8**'
		MOV DWORD PTR [szSearchCode+0002Ch],'8E**'
		MOV BYTE PTR [szSearchCode+00030h],	0
	}
	DWORD pdwCif = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),(CHAR*)szSearchCode,SEARCH_PAGE_EXEC);
	if (pdwCif)
	{
		pdwCif += lstrlenA(szSearchCode) / 2;
		m_dwpMoveAddr = pdwCif + *(DWORD*)(pdwCif) + 4;
		return TRUE;
	}

	return FALSE;
}

DWORD CSearchCodeManager::GetMoveAddr()
{
	return m_dwpMoveAddr;
}

BOOL CSearchCodeManager::SearchMapItemsAddr()
{
	/*
	0062CDFF    A1 AC066A00     mov     eax, dword ptr [6A06AC]          ; »ùÖ·
	0062CE04    8B00            mov     eax, dword ptr [eax]
	0062CE06    8BD3            mov     edx, ebx
	0062CE08    E8 57E3DEFF     call    0041B164                         ; µØÍ¼ÎïÆ·
	0062CE0D    EB 04           jmp     short 0062CE13
	0062CE0F    43              inc     ebx
	0062CE10    4E              dec     esi
	0062CE11  ^ 75 D0           jnz     short 0062CDE3
	0062CE13    5F              pop     edi
	0062CE14    5E              pop     esi
	0062CE15    5B              pop     ebx
	0062CE16    C3              retn
	
	A1????????8B??8B??E8????????EB??434E75??5F5E5BC3
	*/
	CHAR szSearchCode[0x31] = {0};
	__asm
	{
		MOV DWORD PTR [szSearchCode+00000h],'**1A'
		MOV DWORD PTR [szSearchCode+00004h],'****'
		MOV DWORD PTR [szSearchCode+00008h],'B8**'
		MOV DWORD PTR [szSearchCode+0000Ch],'B8**'
		MOV DWORD PTR [szSearchCode+00010h],'8E**'
		MOV DWORD PTR [szSearchCode+00014h],'****'
		MOV DWORD PTR [szSearchCode+00018h],'****'
		MOV DWORD PTR [szSearchCode+0001Ch],'**BE'
		MOV DWORD PTR [szSearchCode+00020h],'E434'
		MOV DWORD PTR [szSearchCode+00024h],'**57'
		MOV DWORD PTR [szSearchCode+00028h],'E5F5'
		MOV DWORD PTR [szSearchCode+0002Ch],'3CB5'
		MOV BYTE PTR [szSearchCode+00030h],0

	}
	DWORD pdwCif = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),(CHAR*)szSearchCode,SEARCH_PAGE_EXEC);
	if (pdwCif)
	{
		if ( ! IsBadReadPtr((DWORD*)(pdwCif+1), 4))
		{
			pdwCif = *(DWORD*)(pdwCif+1);
			if ( ! IsBadReadPtr((DWORD*)pdwCif, 4)
				&& ! IsBadReadPtr((DWORD*)*(DWORD*)pdwCif, 4))
			{
				m_dwMapItemsAddr = *(DWORD*)*(DWORD*)pdwCif;
				return TRUE;
			}
		}
	}
	return FALSE;
}

DWORD CSearchCodeManager::GetMapItemsAddr()
{
	return m_dwMapItemsAddr;
}

BOOL CSearchCodeManager::SearchPeronStructAddr()
{
	/*
	d [[699DEC]]+8 ÈËÎï×ø±ê
	d [[69A424]]	µ±Ç°µØÍ¼Ãû£¨ÒÔ0½áÎ²£©

	0053E41C  5D D7 F8 B1 EA 3A 5B                             ]×ø±ê:[.ÿ



	0053E35A    68 08E45300     push    0053E408                                   ; ASCII "!~ ÎÒÕýÔÚ["
	0053E35F    A1 24A46900     mov     eax, dword ptr [69A424]
	0053E364    FF30            push    dword ptr [eax]
	0053E366    68 1CE45300     push    0053E41C
	0053E36B    8D55 F8         lea     edx, dword ptr [ebp-8]
	0053E36E    A1 EC9D6900     mov     eax, dword ptr [699DEC]
	0053E373    8B00            mov     eax, dword ptr [eax]
	0053E375    0FB740 08       movzx   eax, word ptr [eax+8]
	0053E379    E8 72C6ECFF     call    0040A9F0

	68????????A1????????FF??68????????8D????A1????????8B??0F??????E8
	*/
	CHAR szSearchCode[0x41] = {0};
	__asm
	{
		MOV DWORD PTR [szSearchCode+00000h],'**86'
		MOV DWORD PTR [szSearchCode+00004h],'****'
		MOV DWORD PTR [szSearchCode+00008h],'1A**'
		MOV DWORD PTR [szSearchCode+0000Ch],'****'
		MOV DWORD PTR [szSearchCode+00010h],'****'
		MOV DWORD PTR [szSearchCode+00014h],'**FF'
		MOV DWORD PTR [szSearchCode+00018h],'**86'
		MOV DWORD PTR [szSearchCode+0001Ch],'****'
		MOV DWORD PTR [szSearchCode+00020h],'D8**'
		MOV DWORD PTR [szSearchCode+00024h],'****'
		MOV DWORD PTR [szSearchCode+00028h],'**1A'
		MOV DWORD PTR [szSearchCode+0002Ch],'****'
		MOV DWORD PTR [szSearchCode+00030h],'B8**'
		MOV DWORD PTR [szSearchCode+00034h],'F0**'
		MOV DWORD PTR [szSearchCode+00038h],'****'
		MOV DWORD PTR [szSearchCode+0003Ch],'8E**'
		MOV BYTE PTR [szSearchCode+00040h],0


	}
	DWORD pdwCif = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),(CHAR*)szSearchCode,SEARCH_PAGE_EXEC);
	if (pdwCif)
	{
		pdwCif = *(DWORD*)(pdwCif + lstrlenA("68????????A1????????FF??68????????8D????A1") / 2);
		m_dwPersonStructAddr = *(DWORD*)*(DWORD*)pdwCif;
		return TRUE;
	}
	return FALSE;
}

DWORD CSearchCodeManager::GetPeronStructAddr()
{
	return m_dwPersonStructAddr;
}

BOOL CSearchCodeManager::SearchCallHeroAddr()
{
	/*
	push main/assistant character
	push idx
	push 0
	push 0
	push 0

	00544D4E    6A 01           push    1                                ; 1111
	00544D50    33C0            xor     eax, eax
	00544D52    8AC3            mov     al, bl
	00544D54    50              push    eax
	00544D55    6A 00           push    0
	00544D57    6A 00           push    0
	00544D59    6A 00           push    0
	00544D5B    A1 E0C16A00     mov     eax, dword ptr [6AC1E0]			;	=> begin
	00544D60    8B00            mov     eax, dword ptr [eax]
	00544D62    8B48 04         mov     ecx, dword ptr [eax+4]
	00544D65    A1 04C56A00     mov     eax, dword ptr [6AC504]
	00544D6A    8B00            mov     eax, dword ptr [eax]
	00544D6C    BA 1A040000     mov     edx, 41A
	00544D71    E8 823A0F00     call    006387F8                         ; call hero last

	6A??33??8A??506A??6A??6A??A1????????8B??8B????A1????????8B??BA????????E8		v2.1.1.304
	*/
	DWORD pdwCif = 0;
	CHAR szSearchCode[0x0049] = {0};
	__asm
	{
		MOV DWORD PTR [szSearchCode+00000h],'**A6'
		MOV DWORD PTR [szSearchCode+00004h],'**33'
		MOV DWORD PTR [szSearchCode+00008h],'**A8'
		MOV DWORD PTR [szSearchCode+0000Ch],'A605'
		MOV DWORD PTR [szSearchCode+00010h],'A6**'
		MOV DWORD PTR [szSearchCode+00014h],'A6**'
		MOV DWORD PTR [szSearchCode+00018h],'1A**'
		MOV DWORD PTR [szSearchCode+0001Ch],'****'
		MOV DWORD PTR [szSearchCode+00020h],'****'
		MOV DWORD PTR [szSearchCode+00024h],'**B8'
		MOV DWORD PTR [szSearchCode+00028h],'**B8'
		MOV DWORD PTR [szSearchCode+0002Ch],'1A**'
		MOV DWORD PTR [szSearchCode+00030h],'****'
		MOV DWORD PTR [szSearchCode+00034h],'****'
		MOV DWORD PTR [szSearchCode+00038h],'**B8'
		MOV DWORD PTR [szSearchCode+0003Ch],'**AB'
		MOV DWORD PTR [szSearchCode+00040h],'****'
		MOV DWORD PTR [szSearchCode+00044h],'8E**'
		MOV BYTE PTR [szSearchCode+00048h],0
	}
// 	pdwCif = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),
// 		"53568B**8B**E8********2B****3D********76**E8",SEARCH_PAGE_EXEC);
	pdwCif = SEARCH_EXEC_ANDCLR(szSearchCode);
	if (pdwCif)
	{
		m_dwCallHeroAddr = pdwCif;
		return TRUE;
	}
	return FALSE;
}

DWORD CSearchCodeManager::GetCallHeroAddr()
{
	return m_dwCallHeroAddr;
}

BOOL CSearchCodeManager::SearchRecvMsgAddr()	//ËÑË÷»ØÏÔÏûÏ¢µØÖ·
{
	/*
	00633153    50              push    eax
	00633154    8A43 06         mov     al, byte ptr [ebx+6]
	00633157    E8 2CA2E4FF     call    0047D388
	0063315C    8BC8            mov     ecx, eax
	0063315E    A1 0CDB6A00     mov     eax, dword ptr [6ADB0C]          ; eax È«¾ÖÈ¡Öµ
	00633163    8B00            mov     eax, dword ptr [eax]
	00633165    8B55 FC         mov     edx, dword ptr [ebp-4]
	00633168    E8 7FCDFAFF     call    005DFEEC                         ; »ØÏÔCallÍâ²ã!!!
	0063316D    A1 A0E36A00     mov     eax, dword ptr [6AE3A0]
	00633172    8B00            mov     eax, dword ptr [eax]
	00633174    8B55 FC         mov     edx, dword ptr [ebp-4]
	00633177    E8 D07FFCFF     call    005FB14C

	508A????E8????????8B??A1????????8B??8B????E8????????A1????????8B??8B????E8
	508A****E8********8B**A1********8B**8B****E8********A1********8B**8B****E8

	==±¾µØ»ØÏÔ
	PARAM => ×ÖÌå±³¾°ÑÕÉ«
	EAX => È¡Öµ²ÎÕÕ»ØÏÔCALLÍâ²ã
	ECX => ×ÖÌåÑÕÉ«	[AGBR]
	EDX => ÄÚÈÝ[ALL]
	CHAR szContext[] = {"\x01\x00\x00\x00\x0E\x00\x00\x00this is a test\0"};
	±êÖ¾Î»				·¢ËÍ»ØÏÔÊ¾ÎÄ×ÖµÄ³¤¶È

	005DFEEC    55              push    ebp                              ; ±¾µØ»ØÏÔCall!!
	005DFEED    8BEC            mov     ebp, esp
	005DFEEF    83C4 E0         add     esp, -20
	005DFEF2    53              push    ebx
	005DFEF3    56              push    esi
	005DFEF4    57              push    edi
	005DFEF5    33DB            xor     ebx, ebx
	005DFEF7    895D E0         mov     dword ptr [ebp-20], ebx
	005DFEFA    895D E4         mov     dword ptr [ebp-1C], ebx
	005DFEFD    895D E8         mov     dword ptr [ebp-18], ebx
	005DFF00    895D EC         mov     dword ptr [ebp-14], ebx
	005DFF03    894D F4         mov     dword ptr [ebp-C], ecx
	005DFF06    8955 F8         mov     dword ptr [ebp-8], edx
	005DFF09    8945 FC         mov     dword ptr [ebp-4], eax
	005DFF0C    8B45 F8         mov     eax, dword ptr [ebp-8]
	005DFF0F    E8 944CE2FF     call    00404BA8

	558B??83????53565733??89????89????89????89????89????89????89????8B????E8
	558B**83C4E053565733**89****89****89****89****89****89****89****8B****E8
	*/
	CHAR szSearchCode[0x49] = {0};
	__asm
	{
		MOV DWORD PTR [szSearchCode+00000h],'B855'
		MOV DWORD PTR [szSearchCode+00004h],'38**'
		MOV DWORD PTR [szSearchCode+00008h],'0E4C'
		MOV DWORD PTR [szSearchCode+0000Ch],'6535'
		MOV DWORD PTR [szSearchCode+00010h],'3375'
		MOV DWORD PTR [szSearchCode+00014h],'98**'
		MOV DWORD PTR [szSearchCode+00018h],'****'
		MOV DWORD PTR [szSearchCode+0001Ch],'**98'
		MOV DWORD PTR [szSearchCode+00020h],'98**'
		MOV DWORD PTR [szSearchCode+00024h],'****'
		MOV DWORD PTR [szSearchCode+00028h],'**98'
		MOV DWORD PTR [szSearchCode+0002Ch],'98**'
		MOV DWORD PTR [szSearchCode+00030h],'****'
		MOV DWORD PTR [szSearchCode+00034h],'**98'
		MOV DWORD PTR [szSearchCode+00038h],'98**'
		MOV DWORD PTR [szSearchCode+0003Ch],'****'
		MOV DWORD PTR [szSearchCode+00040h],'**B8'
		MOV DWORD PTR [szSearchCode+00044h],'8E**'
		MOV BYTE PTR [szSearchCode+00048h],0
	}
	m_dwRecvMsgAddr = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(),szSearchCode,SEARCH_PAGE_EXEC);
	if (m_dwRecvMsgAddr)
	{
		return TRUE;
	}
	return FALSE;
}

DWORD CSearchCodeManager::GetRecvMsgAddr()
{
	return m_dwRecvMsgAddr;
}

VOID CSearchCodeManager::Test()
{
	DWORD dwpAddr = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(), 
		"8B**83****53565733**89****89****89****89****89****89****89****8B****E8",	//×î¿ªÊ¼µÄ 55 push ebp ¿ÉÄÜ±»ÐÞ¸Ä³É CC int 3
		SEARCH_PAGE_EXEC);
	if (!dwpAddr)
		MessageBox(0,0,0,0);
	CHAR *pszPushEbp = (CHAR*)(dwpAddr - 1);
	DWORD dwOld = 0;
	if ( VirtualProtect(pszPushEbp, 1, PAGE_EXECUTE_READWRITE, &dwOld) )
	{
		if ( 0xCC == *pszPushEbp )	// int 3
			*pszPushEbp = 0x55;	// push ebp
		dwpAddr -= 1;
		DWORD dwEAX = HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(), 
			"508A****E8********8B**A1********8B**8B****E8********A1********8B**8B****E8",
			SEARCH_PAGE_EXEC);
		dwEAX += lstrlenA("508A****E8********8B**A1") / 2;
		dwEAX = *(DWORD*)dwEAX;
		dwEAX = *(DWORD*)*(DWORD*)dwEAX;
		CHAR szContext[] = {"\x01\x00\x00\x00\x0E\x00\x00\x00this is a test\0"};
		CHAR* pszReal = szContext+8;
		__asm
		{
			pushad
			pushfd
			push 0x00000000
			mov eax, dwEAX
			mov ecx, 0x0000ff00
			mov edx, pszReal
			mov ebx, dwpAddr
			call ebx
			popfd
			popad
		}
		DWORD tmp = 0;
		VirtualProtect(pszPushEbp, 1, dwOld, &tmp);
	}
	
}