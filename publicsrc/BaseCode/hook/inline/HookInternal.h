#pragma once
#define _HOOK_INTERNAL
#pragma pack(1)
#include <windows.h>
#include <shlwapi.h>
#include "../../VM_MACRO/VMP.H"
//#include "../../common/commondef.h"
#include "../../dasm/disassemble.h"
#include "../../system/System.hpp"
#include "../../String/STRING_EX.h"
using namespace STRING_EX;
#pragma comment(lib,"shlwapi")

#pragma warning(disable:4312)
#pragma warning(disable:4099)
#pragma warning(disable:4311)
#pragma warning(disable:4102)
#define NAKED_ALLOC(size) __asm SUB ESP,size
#define NAKED_FREE(size) __asm ADD ESP,size

#define LEAVE_EX(argCount) __asm LEAVE \
	__asm RET argCount*4

#define LEAVE_WITH_ADDR(argCount,retAddr)__asm LEAVE \
	__asm PUSH retAddr \
	__asm POP DWORD PTR[ESP] \
	__asm RET argCount*4

#define FUNCTION_HEAD __asm MOV EDI, EDI __asm PUSH EBP __asm MOV EBP, ESP
#define FUNCTION_ENTRY(argCount) __asm SUB ESP,argCount*4+4 __asm MOV EDI, EDI __asm PUSH EBP __asm MOV EBP, ESP 

#define GET_RET_ADDR(DEST)__asm PUSH DWORD PTR [EBP+4] \
	__asm LEA EAX,DEST \
	__asm POP DWORD PTR [EAX]

#define JMP_MAKE_CALL_ADDR(entry,BackEIP)__asm MOV EAX,BackEIP \
	__asm MOV [EBP+4],EAX \
	__asm MOV EAX,entry \
	__asm JMP EAX 

#define JMP_MAKE_CALL_PTR(entry,BackEIP)__asm MOV EAX,BackEIP \
	__asm MOV [EBP+4],EAX \
	__asm LEA EAX,entry \
	__asm JMP EAX 
#define PUSHALL __asm pushad __asm pushfd
#define POPALL __asm popfd __asm popad

typedef struct tagRESTORE_CODE
{
	int size;
	DWORD RestoreCode[10];
	DWORD JmpBack[10];
	void SaveRestoreCode(DWORD hookAddr,BYTE *RestoreBuff,BOOL brunRestore);

}RESTORE_CODE;
enum SEARCH_PAGE_TYPE
{
	SEARCH_PAGE_RW,
	SEARCH_PAGE_EXEC

};
typedef BOOL (CALLBACK *_HBCallBack)( PEXCEPTION_POINTERS ExceptionInfo );
typedef LONG (CALLBACK *_VirtualVectoredHandler)( PEXCEPTION_POINTERS ExceptionInfo );
typedef struct tagHOOK_INTERNAL
{
	static DWORD SearchAndPlace(BYTE *imageBase,BYTE *dSearch,BYTE *dReplace,DWORD ReplaceLen,BOOL replaceAll);
	static DWORD SearchKeyCode(DWORD Pid,DWORD searchFrom,char *KeyCode,DWORD searchSize=0);
	static DWORD SearchKeyCode(DWORD Pid,char *KeyCode,SEARCH_PAGE_TYPE stype);
	static DWORD SearchKeyCode(DWORD Pid,char *KeyCode,VOID* searchFrom,DWORD rang);
	static ULONG ConvertHexToDec(CHAR* szHex, BYTE nHexLen);
	static DWORD SearchKeyStr(HANDLE hp,BYTE* pbegin,CHAR *fcode,ULONG lenCode,ULONG searchLen,SEARCH_PAGE_TYPE stype);
	_HBCallBack m_HBCallBack;
	_VirtualVectoredHandler m_virtual_function;
	DWORD OldRet;
	DWORD *pHookAddr;	//hook地址
	RESTORE_CODE RestoreCodeMem;//保存原指令
	RESTORE_CODE RestoreCodeFromFile;//原指令
	DWORD *pHookBack;	//返回地址
	DWORD *pCallBack;	//回调函数
	DWORD OffsetByte;
	DWORD *GetFunRealAddr(DWORD *funaddr);
	BYTE pHookAddrCode[512];
	HANDLE hProcess;
	BOOL bRunRestoreCode;
	HINSTANCE ModuleBase;
	tagHOOK_INTERNAL();
	void UnhookAddressCurrentProcess(BOOL fromFile=FALSE);
	void HookAddressCurrentProcess();
	void UnhookAddress(BOOL fromFile=FALSE);
	void HookAddress();
	void InitSoftBP(DWORD* addr,DWORD* newFun);
	DWORD UnhookCurrentProcessHB();
	DWORD InitHookCurrentProcessHB(VOID *HookBaseAddr,_HBCallBack HBCallBack);
	DWORD InitHookCurrentProcess(DWORD offsetbyte,VOID *HookBaseAddr,VOID *newFun,BOOL runRestoreCode);
	DWORD InitHookCurrentProcess(DWORD offsetbyte,TCHAR *module,CHAR *funname,VOID *newFun,BOOL runRestoreCode);
	void InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,char *funname,void *newFun,BOOL runRestoreCode);
	void InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,char *funname,DWORD funOffset,void *newFun,BYTE *restorCode,int len,BOOL runRestoreCode);
	void InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,void *HookBaseAddr,void *newFun,BOOL runRestoreCode);
	BOOL InitHookKeyCode(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,DWORD *newFunc,char* findCode,BOOL runRestoreCode);
	void ReadRestoreCodeFromFile(TCHAR *module);
	VOID InitRestore();
	typedef HANDLE (WINAPI *_CreateEventA)(LPSECURITY_ATTRIBUTES lpEventAttributes,BOOL bManualReset,BOOL bInitialState,LPCSTR lpName);
	typedef BOOL (WINAPI *_SetEvent)(HANDLE hEvent);
	typedef BOOL (WINAPI *_ResetEvent)(HANDLE hEvent);
	typedef DWORD (WINAPI *_WaitForSingleObject)(HANDLE hHandle,DWORD dwMilliseconds);


	_CreateEventA m_CreateEvent;

#define IT_SetEvent(tmpfun,_this,h) __asm nop /*tmpfun = (*_this)->m_SetEvent;\
	__asm PUSH h \
	__asm CALL tmpfun*/

	_SetEvent	  m_SetEvent;

#define IT_ResetEvent(tmpfun,_this,h) __asm nop /*tmpfun = (*_this)->m_ResetEvent;\
	__asm PUSH h \
	__asm CALL tmpfun*/

	_ResetEvent	  m_ResetEvent;
#define IT_WaitForSingleObject(tmpfun,_this,h,t) __asm nop /*tmpfun = (*_this)->m_WaitForSingleObject;\
__asm PUSH t \
__asm PUSH h \
__asm CALL tmpfun*/

	_WaitForSingleObject m_WaitForSingleObject;
	HANDLE m_hEvent;
}HOOK_INTERNAL;
/*
void __declspec(naked)RetBack()
{
	__asm PUSH pHookAddr
		__asm RET

}
void __declspec(naked)JmpBack()
{
	__asm MOV EAX,pHookAddr
		__asm JMP EAX

}
*/
#define SEARCH_EXEC_ANDCLR(sc)	HOOK_INTERNAL::SearchKeyCode(GetCurrentProcessId(), sc,SEARCH_PAGE_EXEC), ZeroMemory(sc, sizeof(sc))

#pragma pack()
