#include "hookInternal.h"
#include "../../String/STRING_EX.h"
#include "../../System/System.hpp"
using namespace SYSTEM_INTERFACE;
#include "../../dasm/disassemble.cpp"
#include <Tlhelp32.h>
#pragma pack(1)
#define MAX_THREAD_ITEM 200
#define WRITE_PAGE_SIZE 100
BYTE opSize[MAX_PATH][MAX_PATH]={NULL}; 

#define ANY_CODE			0x2A2A	//** 作为一个字节的通配符号
static BYTE HookCode=0xE9;
tagHOOK_INTERNAL::tagHOOK_INTERNAL()
{
	m_hEvent=0;
	m_WaitForSingleObject = (_WaitForSingleObject)GetProcAddress(GetModuleHandleA("kernel32.dll"),"WaitForSingleObject");
	m_CreateEvent = (_CreateEventA)GetProcAddress(GetModuleHandleA("kernel32.dll"),"CreateEventA");
	m_SetEvent = (_SetEvent)GetProcAddress(GetModuleHandleA("kernel32.dll"),"SetEvent");;
	m_ResetEvent = (_ResetEvent)GetProcAddress(GetModuleHandleA("kernel32.dll"),"ResetEvent");;
	
	m_virtual_function=0;
	ZeroMemory(pHookAddrCode,sizeof(pHookAddrCode));
}
DWORD tagHOOK_INTERNAL::SearchAndPlace(BYTE *imageBase,BYTE *dSearch,BYTE *dReplace,DWORD ReplaceLen,BOOL replaceAll)
{
	
	BYTE *Pfile;
	DWORD fileSize=0;
	
	BOOL result=FALSE;
	DWORD oldp=0;
	VMBEGIN
	Pfile = imageBase;
	while((DWORD)(Pfile-imageBase)!=(DWORD)(fileSize-ReplaceLen+1))
	{
		if(*Pfile==*dSearch)
		{
			if(!memcmp(Pfile,dSearch,ReplaceLen))
			{
				VirtualProtect(Pfile,ReplaceLen,PAGE_EXECUTE_READWRITE,&oldp);
				CopyMemory(Pfile,dReplace,ReplaceLen);
				result=TRUE;
				VirtualProtect(Pfile,ReplaceLen,oldp,&oldp);
				if(!replaceAll)break;
			}
		}
		Pfile++;
	}
	VMEND
	return result;
	
}
void tagHOOK_INTERNAL::ReadRestoreCodeFromFile(TCHAR *file)
{
	PIMAGE_FILE_HEADER hFs;
	PIMAGE_OPTIONAL_HEADER32 hOp;
	BYTE *dll;
	//BYTE *FileCodePtr;
	BYTE *ImageCodeptr;
	DWORD readout,rdsize;
	HANDLE handle;
	DWORD pCode[3];
	DWORD toffset=0;
	VMBEGIN
	handle = ::CreateFile(file,FILE_GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);

	if(handle!=INVALID_HANDLE_VALUE)
	{
		rdsize = GetFileSize(handle,0);
		dll = (BYTE*)GlobalAlloc(GPTR,rdsize+1);
		ReadFile(handle,dll,rdsize,&readout,0);
		CloseHandle(handle);
		hFs = (PIMAGE_FILE_HEADER)(((PIMAGE_DOS_HEADER)(dll))->e_lfanew+4+(DWORD)dll);
		hOp = (PIMAGE_OPTIONAL_HEADER32)((BYTE*)(hFs)+sizeof(IMAGE_FILE_HEADER));

		ImageCodeptr = (BYTE*)(((DWORD)pHookAddr-(DWORD)hOp->BaseOfCode-(DWORD)ModuleBase)+(DWORD)dll+hOp->SizeOfHeaders);
		if(!IsBadReadPtr(ImageCodeptr,4))
		{
			CopyMemory(pHookAddrCode,ImageCodeptr,sizeof(pHookAddrCode));

			while(toffset<OffsetByte)
			{
				CopyMemory(pCode,(BYTE*)((BYTE*)(pHookAddrCode)+toffset),sizeof(pCode));
				toffset +=GetCodeSize((BYTE*)pCode,(DWORD)pHookAddrCode);
			}
			OffsetByte=toffset;
			RestoreCodeFromFile.SaveRestoreCode((DWORD)pHookAddr,pHookAddrCode+OffsetByte,bRunRestoreCode);

			while(RestoreCodeFromFile.size<5)
			{
				CopyMemory(pCode,(BYTE*)((BYTE*)(RestoreCodeFromFile.RestoreCode)+RestoreCodeFromFile.size),sizeof(pCode));
				RestoreCodeFromFile.size +=GetCodeSize((BYTE*)pCode,(DWORD)RestoreCodeFromFile.RestoreCode);
			}
		}
		GlobalFree(dll);
	}
	VMEND
}



void tagHOOK_INTERNAL::HookAddress()
{
	if(!this->pHookAddr)return;
	DWORD *RestoreCode = this->RestoreCodeMem.RestoreCode;
	DWORD NakedFunc=(DWORD)*(DWORD*)(&this->pCallBack);
	DWORD *xhookAddr = (DWORD*)(DWORD)((DWORD)(this->pHookAddr)+OffsetByte);
	DWORD oldp;
	DWORD temp;
	DWORD rwS;
	VMBEGIN
	if((int)(hProcess)>0)
	{
		VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);

		__asm
		{
#if _DEBUG
			MOV EAX,NakedFunc
			MOV BL,HookCode
			CMP BYTE PTR [EAX],BL
			JNZ nolocalfun
			MOV ECX,[EAX+1]
			LEA EAX,[EAX+ECX+5]
			MOV NakedFunc,EAX
nolocalfun:
#endif
		}

		
		NakedFunc=NakedFunc-(DWORD)xhookAddr-5;
		WriteProcessMemory(hProcess,xhookAddr,&HookCode,1,&rwS);
		WriteProcessMemory(hProcess,(BYTE*)((BYTE*)(xhookAddr)+1),&NakedFunc,4,&rwS);


		VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}else
	{
		VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);

		__asm
		{
#if _DEBUG
			MOV EAX,NakedFunc
			MOV ECX,[EAX+1]
			LEA EAX,[EAX+ECX+5]
			MOV NakedFunc,EAX
#endif
		}
		__asm
		{

			MOV ESI,RestoreCode
			MOV EDI,xhookAddr
			MOV EAX,[EDI]
			CMP AL,HookCode
			JZ hooked2

		}
		__asm
		{
			MOV EAX,xhookAddr
			SUB NakedFunc,EAX

		}
		NakedFunc -=5;

		__asm
		{
			MOV AL,HookCode
			MOV BYTE PTR [EDI],AL
			MOV EAX,NakedFunc
			MOV DWORD PTR [EDI+1],EAX

		}
hooked2:

		VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}
	VMEND
	return;
	

}
void tagHOOK_INTERNAL::UnhookAddress(BOOL fromFile)
{
	if(!this->pHookAddr)return;
	if( IsBadReadPtr((BYTE*)this->pHookAddr, 4) || *(BYTE*)this->pHookAddr!=HookCode)return;
	DWORD oldp;
	DWORD temp;
	DWORD csize;
	DWORD *RestoreCode;
	DWORD *xhookAddr;
	VMBEGIN
	csize = this->RestoreCodeMem.size;
	RestoreCode = this->RestoreCodeMem.RestoreCode;
	if(fromFile)
	{
		RestoreCode = this->RestoreCodeFromFile.RestoreCode;
		csize = this->RestoreCodeFromFile.size;
	}
	xhookAddr = (DWORD*)(DWORD)((DWORD)(this->pHookAddr)+OffsetByte);
    
	if((int)(hProcess)>0)
	{
		VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);
		WriteProcessMemory(hProcess,xhookAddr,RestoreCode,8,&oldp);
		VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}
	else
	{
		VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);
		__asm
		{

			MOV ESI,RestoreCode
			MOV EDI,xhookAddr
			MOV EAX,[ESI]
			MOV [EDI],EAX
			MOV EAX,[ESI+4]
			MOV [EDI+4],EAX
		}

		VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}
	VMEND
}
void tagHOOK_INTERNAL::InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,char *function,DWORD funOffset,void *newFun,BYTE *restorCode,int len,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	OffsetByte = offsetbyte;
	ModuleBase = GetModuleHandle(module);
	if(!ModuleBase)ModuleBase=LoadLibrary(module);
	pHookAddr = (DWORD*)(BYTE*)((BYTE*)(GetProcAddress(ModuleBase,function))+funOffset);
	pCallBack = (DWORD*)newFun;
	CopyMemory((BYTE*)restorCode,(BYTE*)pHookAddr,len);
	ReadRestoreCodeFromFile(module);
	InitRestore();
	HookAddress();
}
void tagHOOK_INTERNAL::InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,char *function,void *newFun,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	OffsetByte = offsetbyte;
	ModuleBase = GetModuleHandle(module);
	if(!ModuleBase)ModuleBase=LoadLibrary(module);
	pHookAddr = (DWORD*)(GetProcAddress(ModuleBase,function));
	pCallBack = (DWORD*)newFun;
	ReadRestoreCodeFromFile(module);
	InitRestore();
	HookAddress();
}
VOID tagHOOK_INTERNAL::InitRestore()
{
//	DWORD olptr;
	DWORD pCode[3];
	//int codeLen=0;
	
	//计算实际的offset
	DWORD toffset=0;
	CopyMemory(pHookAddrCode,pHookAddr,sizeof(pHookAddrCode));
	if(!*pHookAddrCode)return;
	while(toffset<OffsetByte)
	{
		CopyMemory(pCode,(BYTE*)((BYTE*)(pHookAddrCode)+toffset),sizeof(pCode));
		toffset +=GetCodeSize((BYTE*)pCode,(DWORD)pHookAddrCode);
	}
	OffsetByte=toffset;

	RestoreCodeMem.SaveRestoreCode((DWORD)((BYTE*)(pHookAddr)+OffsetByte),pHookAddrCode+OffsetByte,bRunRestoreCode);

	
	while(RestoreCodeMem.size<5)
	{
		CopyMemory(pCode,(BYTE*)((BYTE*)(RestoreCodeMem.RestoreCode)+RestoreCodeMem.size),sizeof(pCode));
		RestoreCodeMem.size +=GetCodeSize((BYTE*)pCode,(DWORD)RestoreCodeMem.RestoreCode);
	}
}

BOOL tagHOOK_INTERNAL::InitHookKeyCode(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,DWORD *newFunc,char* findCode,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	hProcess = _hProcess;
	OffsetByte = offsetbyte;
	ModuleBase = GetModuleHandle(module);
	if(!ModuleBase)ModuleBase=LoadLibrary(module);
	pHookAddr = (DWORD *) SearchKeyCode(GetCurrentProcessId(),(DWORD)ModuleBase,findCode);
	pCallBack = (DWORD*)newFunc;
	ReadRestoreCodeFromFile(module);
	InitRestore();
	HookAddress();
	
return TRUE;
}
void tagHOOK_INTERNAL::InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,void *HookBaseAddr,void *newFun,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	hProcess = _hProcess;
	OffsetByte = offsetbyte;
	MEMORY_BASIC_INFORMATION mbi={NULL};
	VirtualQuery((BYTE*)HookBaseAddr,&mbi,sizeof(MEMORY_BASIC_INFORMATION));
	ModuleBase=(HINSTANCE)mbi.AllocationBase;
	pHookAddr = (DWORD*)HookBaseAddr;
	pCallBack = (DWORD*)newFun;
	ReadRestoreCodeFromFile(module);
	InitRestore();
	HookAddress();
	
}

DWORD *tagHOOK_INTERNAL::GetFunRealAddr(DWORD *funaddr)
{
	DWORD *dest=0;
	__asm
	{
#if _DEBUG
		MOV EAX,funaddr
		MOV ECX,[EAX+1]
		LEA EAX,[EAX+ECX+5]
#else
		MOV EAX,funaddr
#endif
		LEA ESI,dest
		MOV [ESI],EAX
	}
	return dest;
}
DWORD tagHOOK_INTERNAL::SearchKeyCode(DWORD Pid,char *KeyCode,VOID* searchFrom,DWORD rang)
{
	DWORD ret=0;
	DWORD	cntSearch=0,cntCmp=0;
	CHAR	*ptr;
	BYTE	*pCode;
	BOOL	isgo = FALSE;
	DWORD   lenCode = lstrlenA(KeyCode);
	DWORD	searchRang = rang;
	DWORD   cnt=0;
	ptr = KeyCode;
	pCode = (BYTE*)searchFrom;
	//if(!pCode)return 0;
	//转换16进制
	BYTE *hexcode = (BYTE*)malloc(lenCode>>1);
	for(cnt=0;cnt<lenCode;cnt+=2)
	{
		if(*(WORD*)((BYTE*)(ptr)+cnt)==ANY_CODE)
		{
			hexcode[cnt>>1]=0;
			continue;
		}
		hexcode[cnt>>1]=(BYTE)ConvertHexToDec((CHAR*)(ptr+cnt),1);
	}
	///枚举所有虚拟内存页

	for(cntSearch=0;cntSearch<searchRang;cntSearch++)
	{
		for(cntCmp=0;cntCmp<lenCode;cntCmp+=2)
		{

			if(*(WORD*)((BYTE*)(ptr)+cntCmp)==ANY_CODE)
			{
				continue;
			}
			if(0==pCode)
			{
				//__asm int 3
				cntSearch=searchRang;
				break;
			}
			if((searchRang-cntSearch)<(lenCode>>1))
			{
				cntSearch=searchRang;
				break;
			}
			if(pCode[cntSearch+(cntCmp>>1)]==hexcode[cntCmp>>1])
			{
				if((cntCmp+2==lenCode) && (hexcode!=(BYTE*)(pCode+cntSearch)))
				{
					free(hexcode);
					return (DWORD)(BYTE*)(pCode+cntSearch);
				}
			}else
			{
				break;
			}

		}
	}

	free(hexcode);
	return 0;
	return ret;
}
DWORD tagHOOK_INTERNAL::SearchKeyCode(DWORD Pid,char *KeyCode,SEARCH_PAGE_TYPE stype)
{

	DWORD ret=0;
	char *cmpcode=0;
	//HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);
//	if(!searchSize)searchSize=0x0080000;



// 	if(Pid==GetCurrentProcessId())
// 	{
		//cmpcode = (char*)searchFrom;
		ret = SearchKeyStr(GetCurrentProcess(),(BYTE*)cmpcode,KeyCode,lstrlenA(KeyCode),0,stype);
// 	}else
// 	{
// 		cmpcode=(char*)malloc(searchSize);
// 		ZeroMemory(cmpcode,searchSize);
// 		ReadProcessMemory(hProcess,(LPCVOID)searchFrom,cmpcode,searchSize,0);
// 		ret = SearchKeyStr(hProcess,(BYTE*)cmpcode,KeyCode,lstrlenA(KeyCode),0,SEARCH_PAGE_RW);
// 		free(cmpcode);
// 	}

	//CloseHandle(hProcess);
	return ret;
}
DWORD tagHOOK_INTERNAL::SearchKeyCode(DWORD Pid,DWORD searchFrom,char *KeyCode,DWORD searchSize)
{
	
	DWORD ret=0;
	char *cmpcode=0;
	HANDLE hProcess=OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);
	if(!searchSize)searchSize=0x0080000;

	
	
	if(Pid==GetCurrentProcessId())
	{
		cmpcode = (char*)searchFrom;
		ret = SearchKeyStr(GetCurrentProcess(),(BYTE*)cmpcode,KeyCode,lstrlenA(KeyCode),searchSize-4,SEARCH_PAGE_EXEC);
	}else
	{
		cmpcode=(char*)malloc(searchSize);
		ZeroMemory(cmpcode,searchSize);
		ReadProcessMemory(hProcess,(LPCVOID)searchFrom,cmpcode,searchSize,0);
		ret = SearchKeyStr(hProcess,(BYTE*)cmpcode,KeyCode,lstrlenA(KeyCode),searchSize-4,SEARCH_PAGE_EXEC);
		free(cmpcode);
	}

	CloseHandle(hProcess);
	return ret;
}
ULONG tagHOOK_INTERNAL::ConvertHexToDec(CHAR* szHex, BYTE nHexLen)
{
	CHAR c = 0;
	int nShift = 0;
	ULONG nDec = 0;
	nHexLen <<=1;
	if(szHex[nHexLen-1]=='*')return 0;
	while ( nHexLen > 0 )
	{

		c = szHex[nHexLen-1];
		if ( c>='0' && c<='9' )
		{
			nDec += ((ULONG)(c-'0')) << nShift;
		}
		else if ( c>='A' && c<='F' )
		{
			nDec += ((ULONG)(10+c-'A')) << nShift;
		}
		else if ( c>='a' && c<='f' )
		{
			nDec += ((ULONG)(10+c-'a')) << nShift;
		}
		--nHexLen;
		nShift += 4;
	}
	return nDec;
}
BOOL WINAPI EnumVirtualMemoryEx(HANDLE hp,DWORD beginaddr,MEMORY_BASIC_INFORMATION *t,SEARCH_PAGE_TYPE stype)
{  
	VOID * pAddress=(VOID*)beginaddr;  
	
	DWORD dwResult=0;  
	do
	{
		dwResult=VirtualQueryEx(hp,pAddress,t,sizeof(MEMORY_BASIC_INFORMATION));  
		if((stype==SEARCH_PAGE_RW) && ((t->Protect==PAGE_WRITECOMBINE)||(t->Protect==PAGE_WRITECOPY)||(t->Protect==PAGE_READONLY)||(t->Protect==PAGE_READWRITE)))
		{
			return TRUE;
		}
		if((stype==SEARCH_PAGE_EXEC) && ((t->Protect==PAGE_EXECUTE)||(t->Protect==PAGE_EXECUTE_READ)||(t->Protect==PAGE_EXECUTE_READWRITE)||(t->Protect==PAGE_EXECUTE_WRITECOPY)))
		{
			return TRUE;
		}
		pAddress=(PBYTE)pAddress+t->RegionSize;  

	}while(dwResult);
	return FALSE;
}  
DWORD tagHOOK_INTERNAL::SearchKeyStr(HANDLE hp,BYTE* pbegin,CHAR *fcode,ULONG lenCode,ULONG searchLen,SEARCH_PAGE_TYPE stype)
{
	DWORD	cntSearch=0,cntCmp=0;
	CHAR	*ptr;
	BYTE	*pCode;
	BOOL	isgo = FALSE;
	DWORD	rlen = lenCode/2;
	MEMORY_BASIC_INFORMATION t={0};
	ptr = fcode;
	pCode = (BYTE*)pbegin;
	//if(!pCode)return 0;
	//转换16进制
	BYTE *hexcode = (BYTE*)malloc(lenCode>>1);
	for(int cnt=0;cnt<lenCode;cnt+=2)
	{
		if(*(WORD*)((BYTE*)(ptr)+cnt)==ANY_CODE)
		{
			hexcode[cnt>>1]=0;
			continue;
		}
		hexcode[cnt>>1]=(BYTE)ConvertHexToDec((CHAR*)(ptr+cnt),1);
	}
	///枚举所有虚拟内存页
	#ifndef LOLINTERFACETESTDLL_EXPORTS	//测试工程标记 
	try
	{
	#endif
		do 
		{
			isgo = EnumVirtualMemoryEx(hp,(DWORD)pCode,&t,stype);
			if(!isgo)
			{
				free(hexcode);
				return 0;
			}
			pCode = (BYTE*)t.BaseAddress;
			searchLen = t.RegionSize;
			for(cntSearch=0;cntSearch<searchLen;cntSearch++)
			{
				for(cntCmp=0;cntCmp<lenCode;cntCmp+=2)
				{

					if(*(WORD*)((BYTE*)(ptr)+cntCmp)==ANY_CODE)
					{
						continue;
					}
					if(0==pCode)
					{
						//__asm int 3
						cntSearch=searchLen;
						break;
					}
					if((searchLen-cntSearch)<(lenCode>>1))
					{
						cntSearch=searchLen;
						break;
					}
					if(pCode[cntSearch+(cntCmp>>1)]==hexcode[cntCmp>>1])
					{
						if((cntCmp+2==lenCode) && (hexcode!=(BYTE*)(pCode+cntSearch)))
						{
							free(hexcode);
							return (DWORD)(BYTE*)(pCode+cntSearch);
						}
					}else
					{
						break;
					}

				}
			}
			pCode = (BYTE*)((DWORD)(t.BaseAddress)+t.RegionSize);
		} while (isgo);
#ifndef LOLINTERFACETESTDLL_EXPORTS	//测试工程标记 
	}
	catch (...)
	{
		//e->Delete();
	}
#endif
	free(hexcode);
	return 0;
}

void tagHOOK_INTERNAL::HookAddressCurrentProcess()
{
	if(!this->pHookAddr)return;
	DWORD *RestoreCode = this->RestoreCodeMem.RestoreCode;
	DWORD NakedFunc;
	DWORD *xhookAddr;
	DWORD oldp;
	DWORD temp;
	VMBEGIN
	RestoreCode = this->RestoreCodeMem.RestoreCode;
	NakedFunc=(DWORD)*(DWORD*)(&this->pCallBack);
	xhookAddr = (DWORD*)(DWORD)((DWORD)(this->pHookAddr)+OffsetByte);
	VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);

		__asm
		{
#if _DEBUG
			MOV EAX,NakedFunc
			MOV ECX,[EAX+1]
			LEA EAX,[EAX+ECX+5]
			MOV NakedFunc,EAX
#endif
		}
		__asm
		{

			MOV ESI,RestoreCode
				MOV EDI,xhookAddr
				MOV EAX,[EDI]
			CMP AL,HookCode
				JZ hooked2

		}
		__asm
		{
			MOV EAX,xhookAddr
				SUB NakedFunc,EAX

		}
		NakedFunc-=5;

		__asm
		{
			MOV AL,HookCode
			MOV BYTE PTR [EDI],AL
			MOV EAX,NakedFunc
			MOV DWORD PTR [EDI+1],EAX

		}
hooked2:

	VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);

	VMEND
	return;
	

}
void tagHOOK_INTERNAL::UnhookAddressCurrentProcess(BOOL fromFile)
{
	if(!this->pHookAddr)return;
	DWORD csize;
	DWORD *RestoreCode;
	DWORD *xhookAddr;
	DWORD oldp;
	DWORD temp;

	VMBEGIN
	csize = this->RestoreCodeMem.size;
	RestoreCode = this->RestoreCodeMem.RestoreCode;
	if(fromFile)
	{
		RestoreCode = this->RestoreCodeFromFile.RestoreCode;
		csize = this->RestoreCodeFromFile.size;
	}
	xhookAddr = (DWORD*)(DWORD)((DWORD)(this->pHookAddr)+OffsetByte);

	VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);
	__asm
	{

		MOV ESI,RestoreCode
		MOV EDI,xhookAddr
		MOV EAX,[ESI]
		MOV [EDI],EAX
		MOV EAX,[ESI+4]
		MOV [EDI+4],EAX
	}

	VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
VMEND

}

BOOL ListProcessThreads(DWORD dwOwnerPID,DWORD *threadList) 
{ 
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE; 
	THREADENTRY32 te32; 

	hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 ); 
	if( hThreadSnap == INVALID_HANDLE_VALUE ) 
		return FALSE; 
	te32.dwSize = sizeof(THREADENTRY32 ); 

	if( !Thread32First( hThreadSnap, &te32 ) ) 
	{
		CloseHandle( hThreadSnap );
		return FALSE;
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do 
	{ 
		if( te32.th32OwnerProcessID == dwOwnerPID )
		{
			*threadList = te32.th32ThreadID;
			threadList++;
			//printf( "\n\n     THREAD ID      = 0x%08X", te32.th32ThreadID ); 
			//printf( "\n     Base priority  = %d", te32.tpBasePri ); 
			//printf( "\n     Delta priority = %d", te32.tpDeltaPri ); 
			//MessageBoxA(0,0,0,0);
		}
	} while( Thread32Next(hThreadSnap, &te32 ) ); 

	CloseHandle( hThreadSnap );
	return TRUE;
}
enum HB_DR_FLAG
{
	HB_NONE=0,
	HB_DR0=1,
	HB_DR1=0x04,
	HB_DR2=0x10,
	HB_DR3=0x40
};
#define REV_FLAG 0x400;
#define MSGBOX(x) __asm nop;//MessageBoxA(0,x,0,0);

//使用硬件hook把c/c++编译选项的。"基本运行时检查"设置为"未初始化的变量",如下采用汇编混合的写法逃避编译器插入堆栈检查函数
LONG __declspec(naked) CALLBACK VectoredHandler(PEXCEPTION_POINTERS ExceptionInfo )
{
	//CHAR outs[MAX_PATH];
	//ExceptionInfo->ContextRecord->Eip++;
	LONG result;
	HB_DR_FLAG *SINGLE_STEP;
	_HBCallBack callback;
	VOID* tmpfunc;
	tagHOOK_INTERNAL **_this;
	HANDLE hevent;
	//开辟代码内静态变量,以下指令生成8个0
	__asm
	{
		PUSH EBP
		MOV EBP,ESP
		SUB ESP,0x400
		PUSHAD
	}
	__asm
	{

		JMP varend
static_var:
		ADD BYTE PTR [EAX], AL
		ADD BYTE PTR [EAX], AL
hookInternal_this:
		ADD BYTE PTR [EAX], AL
		ADD BYTE PTR [EAX], AL
varend:
		CALL $+5
		POP EAX
		PUSH EAX
		LEA EAX,[EAX-5-8]	//得到第一个变量的地址
		MOV SINGLE_STEP,EAX
		POP EAX
		LEA EAX,[EAX-5-4]	//得到第二个变量的地址
		MOV _this,EAX
	}
	if(ExceptionInfo->ContextRecord->Eip==0xFFFFFFFF)
	{
		*(DWORD*)_this = ExceptionInfo->ContextRecord->Eax;
		*SINGLE_STEP = HB_NONE;
		__asm
		{
			POPAD
			XOR EAX,EAX
			LEAVE
			RET 4
		}
	}
	hevent = (*_this)->m_hEvent;
	//__asm int 3
	IT_WaitForSingleObject(tmpfunc,_this,hevent,-1);
	IT_ResetEvent(tmpfunc,_this,hevent);
	//变量赋值开始
	result=EXCEPTION_CONTINUE_EXECUTION;//EXCEPTION_CONTINUE_EXECUTION;//EXCEPTION_CONTINUE_SEARCH
	if(ExceptionInfo->ExceptionRecord->ExceptionCode==EXCEPTION_SINGLE_STEP)
	{
		//if((0!=(ExceptionInfo->ContextRecord->EFlags & 0x100)) && (//*SINGLE_STEP!=HB_NONE))

			//判断dr7
			
			if((DWORD)ExceptionInfo->ContextRecord->Eip != (DWORD)(*_this)->pHookAddr)
			{
				//if(*SINGLE_STEP!=HB_NONE)
				//{
					
					if((DWORD)(*_this)->pHookAddr ==ExceptionInfo->ContextRecord->Dr0)
					//if(HB_DR0==*SINGLE_STEP)
					{
						ExceptionInfo->ContextRecord->Dr0 = (DWORD)(*_this)->pHookAddr;
						ExceptionInfo->ContextRecord->Dr7 |=HB_DR0;
					}
					if((DWORD)(*_this)->pHookAddr ==ExceptionInfo->ContextRecord->Dr1)
					//if(HB_DR1==*SINGLE_STEP)
					{
						ExceptionInfo->ContextRecord->Dr1 = (DWORD)(*_this)->pHookAddr;
						ExceptionInfo->ContextRecord->Dr7 |=HB_DR1;
					}
					if((DWORD)(*_this)->pHookAddr ==ExceptionInfo->ContextRecord->Dr2)
					//if(HB_DR2==*SINGLE_STEP)
					{
						ExceptionInfo->ContextRecord->Dr2 = (DWORD)(*_this)->pHookAddr;
						ExceptionInfo->ContextRecord->Dr7 |=HB_DR2;
					}
					if((DWORD)(*_this)->pHookAddr ==ExceptionInfo->ContextRecord->Dr3)
					//if(HB_DR3==*SINGLE_STEP)
					{
						ExceptionInfo->ContextRecord->Dr3 = (DWORD)(*_this)->pHookAddr;
						ExceptionInfo->ContextRecord->Dr7 |=HB_DR3;
					}
					//MSGBOX("00");
					
					ExceptionInfo->ContextRecord->EFlags = 0;
					ExceptionInfo->ContextRecord->Dr6=0;
					
				
					*SINGLE_STEP=HB_NONE;
					IT_SetEvent(tmpfunc,_this,hevent);
					__asm
					{
						POPAD
						MOV EAX,result
						LEAVE
						RET 4
					}
				//}
		}
	
		if((DWORD)(*_this)->pHookAddr==(DWORD)ExceptionInfo->ContextRecord->Eip)
		{
			if((DWORD)(*_this)->pHookAddr==ExceptionInfo->ContextRecord->Dr0)
			{
				*SINGLE_STEP=HB_DR0;
				ExceptionInfo->ContextRecord->Dr7 ^=HB_DR0;
			}
			if((DWORD)(*_this)->pHookAddr==ExceptionInfo->ContextRecord->Dr1)
			{
				*SINGLE_STEP=HB_DR1;
				ExceptionInfo->ContextRecord->Dr7 ^=HB_DR1;
			}
			if((DWORD)(*_this)->pHookAddr==ExceptionInfo->ContextRecord->Dr2)
			{
				*SINGLE_STEP=HB_DR2;
				ExceptionInfo->ContextRecord->Dr7 ^=HB_DR2;
			}
			if((DWORD)(*_this)->pHookAddr==ExceptionInfo->ContextRecord->Dr3)
			{
				*SINGLE_STEP=HB_DR3;
				ExceptionInfo->ContextRecord->Dr7 ^=HB_DR3;
			}
			ExceptionInfo->ContextRecord->EFlags |= 0x100 ;//单步模式
			if((*_this)->m_HBCallBack)
			{
				callback = (*_this)->m_HBCallBack;
				//(*_this)->m_HBCallBack(ExceptionInfo);
				__asm
				{
					MOV EAX,ExceptionInfo
					PUSH EAX
					MOV EAX,callback
					CALL EAX
				}
			}
		}else 
		{
			IT_SetEvent(tmpfunc,_this,hevent);
			__asm
			{
				POPAD
				MOV EAX,EXCEPTION_CONTINUE_SEARCH
				LEAVE
				RET 4
			}
		}
	}else
	{
		IT_SetEvent(tmpfunc,_this,hevent);
		__asm
		{
			POPAD
			MOV EAX,EXCEPTION_EXECUTE_HANDLER
			LEAVE
			RET 4
		}

	}
IT_SetEvent(tmpfunc,_this,hevent);
	__asm
	{
		
		POPAD
		MOV EAX,result
		LEAVE
		RET 4
	}
}
DWORD tagHOOK_INTERNAL::UnhookCurrentProcessHB()
{
	CONTEXT context={0};
	HANDLE th;
	DWORD threadList[MAX_THREAD_ITEM]={0};
	INT cnt=0;
	//if(!HookBaseAddr || !HBCallBack)return 0;

	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;

	ListProcessThreads(GetCurrentProcessId(),threadList);
	for(cnt=0;cnt<MAX_THREAD_ITEM;cnt++)
	{
		if(threadList[cnt])
		{
			th = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME,FALSE,threadList[cnt]);
			if(th)
			{
				GetThreadContext(th,&context);
				//找一个空的dr寄存器写入断点
				if(context.Dr0==(DWORD)pHookAddr)
				{
					context.Dr0 = 0;
					if(context.Dr7&HB_DR0)
						context.Dr7 -= HB_DR0;
				}else if(context.Dr1==(DWORD)pHookAddr)
				{
					context.Dr1 = 0;
					if(context.Dr7&HB_DR1)
						context.Dr7 -= HB_DR1;
				}else if(context.Dr2==(DWORD)pHookAddr)
				{
					context.Dr2 = 0;
					if(context.Dr7&HB_DR2)
						context.Dr7 -= HB_DR2;
				}else if(context.Dr3==(DWORD)pHookAddr)
				{
					context.Dr3 = 0;
					if(context.Dr7&HB_DR3)
						context.Dr7 -= HB_DR3;
				}
				context.Dr6 = 0;
				SetThreadContext(th,&context);
				CloseHandle(th);
			}
		}
	}
	return context.Dr7;
}
BYTE* MakeVirtualCode(BYTE *srcCode,DWORD codeSize)
{
	BYTE* execPage = (BYTE*)VirtualAlloc(0,codeSize,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if(!execPage)return 0;
	CopyMemory(execPage,srcCode,codeSize);
	return execPage;
}

DWORD tagHOOK_INTERNAL::InitHookCurrentProcessHB(VOID *HookBaseAddr,_HBCallBack HBCallBack)
{
	CONTEXT context={0};
	HANDLE th;
	DWORD threadList[MAX_THREAD_ITEM]={0};
	INT cnt=0;
	//if(!HookBaseAddr || !HBCallBack)return 0;

	context.ContextFlags = CONTEXT_DEBUG_REGISTERS;//CONTEXT_FULL | CONTEXT_DEBUG_REGISTERS;

	ListProcessThreads(GetCurrentProcessId(),threadList);
	m_HBCallBack = HBCallBack;
	pHookAddr = (DWORD*)HookBaseAddr;
	for(cnt=0;cnt<MAX_THREAD_ITEM;cnt++)
	{
		//__asm int 3
		if(threadList[cnt] && threadList[cnt]!=GetCurrentThreadId())
		{
			th = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME,FALSE,threadList[cnt]);
			if(th)
			{
				SuspendThread(th);
				GetThreadContext(th,&context);
				//判断之前是否有断点
				if(context.Dr0==(DWORD)pHookAddr)
				{
					return 0;
				}else if(context.Dr1==(DWORD)pHookAddr)
				{
					return 0;
				}else if(context.Dr2==(DWORD)pHookAddr)
				{
					return 0;
				}else if(context.Dr3==(DWORD)pHookAddr)
				{
					return 0;
				}
				//找一个空的dr寄存器写入断点
				if(!context.Dr0)
				{
					context.Dr0 = (DWORD)pHookAddr;
					context.Dr7 |= HB_DR0;

				}else if(!context.Dr1)
				{
					context.Dr1 = (DWORD)pHookAddr;
					context.Dr7 |= HB_DR1;

				}else if(!context.Dr2)
				{
					context.Dr2 = (DWORD)pHookAddr;
					context.Dr7 |= HB_DR2;

				}else if(!context.Dr3)
				{
					context.Dr3 = (DWORD)pHookAddr;
					context.Dr7 |= HB_DR3;

				}

				context.Dr6 = 0;
				if(!m_virtual_function)
				{
					m_hEvent = m_CreateEvent(0,TRUE,TRUE,0);
					#ifdef _DEBUG
							m_virtual_function = (_VirtualVectoredHandler)MakeVirtualCode((BYTE*)GetFunRealAddr((DWORD*)&VectoredHandler),1024*5);
					#else
							m_virtual_function = (_VirtualVectoredHandler)MakeVirtualCode((BYTE*)&VectoredHandler,1024*5);
					#endif

					//RemoveVectoredExceptionHandler(&VectoredHandler);
					//初始化函数内变量
					CONTEXT ContextRecord;
					EXCEPTION_POINTERS ep;
					ep.ContextRecord = &ContextRecord;
					ep.ContextRecord->Eip=0xFFFFFFFF;
					ep.ContextRecord->Eax=(DWORD)this;
					//VectoredHandler(&ep);
					m_virtual_function(&ep);

					AddVectoredExceptionHandler(0,m_virtual_function);

					//AddVectoredContinueHandler(1,&VectoredHandler);
					//AddHB_PTR(this);
				}
				
				SetThreadContext(th,&context);
				CloseHandle(th);
			}
		}
	}
	//恢复所有线程
	for(cnt=0;cnt<MAX_THREAD_ITEM;cnt++)
	{
		if(threadList[cnt] && threadList[cnt]!=GetCurrentThreadId())
		{
			th = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME,FALSE,threadList[cnt]);
			if(th)
			{
				ResumeThread(th);
				CloseHandle(th);
			}
			
		}
	}
	return context.Dr7;
}
//DWORD tagHOOK_INTERNAL::InitHookCurrentProcessHB(VOID *HookBaseAddr,_HBCallBack HBCallBack)
//{
//	CONTEXT context={0};
//	HANDLE th;
//	DWORD threadList[MAX_THREAD_ITEM]={0};
//	INT cnt=0;
//	if(!HookBaseAddr || !HBCallBack)return 0;
//
//	context.ContextFlags = CONTEXT_FULL| CONTEXT_DEBUG_REGISTERS;
//	
//	ListProcessThreads(GetCurrentProcessId(),threadList);
//	m_HBCallBack = HBCallBack;
//	pHookAddr = (DWORD*)HookBaseAddr;
//	for(cnt=0;cnt<MAX_THREAD_ITEM;cnt++)
//	{
//		if(threadList[cnt])
//		{
//			th = OpenThread(THREAD_ALL_ACCESS,FALSE,threadList[cnt]);
//			GetThreadContext(th,&context);
//			//判断之前是否有断点
//			if(context.Dr0==(DWORD)pHookAddr)
//			{
//				return 0;
//			}else if(context.Dr1==(DWORD)pHookAddr)
//			{
//				return 0;
//			}else if(context.Dr2==(DWORD)pHookAddr)
//			{
//				return 0;
//			}else if(context.Dr3==(DWORD)pHookAddr)
//			{
//				return 0;
//			}
//			//找一个空的dr寄存器写入断点
//			if(!context.Dr0)
//			{
//				context.Dr0 = (DWORD)pHookAddr;
//				context.Dr7 |= HB_DR0;
//				AddVectoredExceptionHandler(0,&VectoredHandler);
//			}else if(!context.Dr1)
//			{
//				context.Dr1 = (DWORD)pHookAddr;
//				context.Dr7 |= HB_DR1;
//				AddVectoredExceptionHandler(0,&VectoredHandler);
//			}else if(!context.Dr2)
//			{
//				context.Dr2 = (DWORD)pHookAddr;
//				context.Dr7 |= HB_DR2;
//				AddVectoredExceptionHandler(0,&VectoredHandler);
//			}else if(!context.Dr3)
//			{
//				context.Dr3 = (DWORD)pHookAddr;
//				context.Dr7 |= HB_DR3;
//				AddVectoredExceptionHandler(0,&VectoredHandler);
//			}
//			context.Dr6 = 0;
//			SetThreadContext(th,&context);
//			if(th)CloseHandle(th);
//		}
//	}
//	return context.Dr7;
//}
DWORD tagHOOK_INTERNAL::InitHookCurrentProcess(DWORD offsetbyte,VOID *HookBaseAddr,VOID *newFun,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	OffsetByte = offsetbyte;
	ModuleBase = GetModuleHandle(0);
	pHookAddr = (DWORD*)HookBaseAddr;
	pCallBack = (DWORD*)newFun;
	InitRestore();
	HookAddressCurrentProcess();
	return (DWORD)pHookAddr;
}
DWORD tagHOOK_INTERNAL::InitHookCurrentProcess(DWORD offsetbyte,TCHAR *module,CHAR *funname,VOID *newFun,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	hProcess = 0;
	OffsetByte = offsetbyte;
	
	ModuleBase = GetModuleHandle(module);
	
	if(!ModuleBase)ModuleBase=LoadLibrary(module);
	pHookAddr = (DWORD*)(GetProcAddress(ModuleBase,funname));
	pCallBack = (DWORD*)newFun;
	InitRestore();
	HookAddressCurrentProcess();
	return (DWORD)pHookAddr;
}
void RESTORE_CODE::SaveRestoreCode(DWORD hookAddr,BYTE *RestoreBuff,BOOL brunRestore)
{
	{
		DWORD rsSize=0;
		DWORD pCode[3]={0};
		DWORD olptr=0;
		CopyMemory(RestoreCode,RestoreBuff,sizeof(RestoreCode));
		CopyMemory(JmpBack,RestoreBuff,sizeof(JmpBack));
		VirtualProtect(JmpBack,sizeof(JmpBack),PAGE_EXECUTE_READWRITE,&olptr);
		//GetCodeSize((BYTE*)pCode,(DWORD)pHookAddrCode);


		while(rsSize<5)
		{
			CopyMemory(pCode,(BYTE*)((BYTE*)(RestoreBuff)+rsSize),sizeof(pCode));
			rsSize +=GetCodeSize((BYTE*)pCode,(DWORD)RestoreBuff);
		}

		BYTE *Writejmp=0;
		Writejmp = (BYTE*)JmpBack;
		if(brunRestore)
		{
			size +=rsSize;
			*(DWORD*)(Writejmp+size+1)= ((DWORD)hookAddr)- ((DWORD)(Writejmp))-5;
		}else
		{
			*(DWORD*)(Writejmp+size+1)= ((DWORD)hookAddr+rsSize)- ((DWORD)(Writejmp))-5;
		}

		Writejmp[size]=HookCode;


	}
}
#pragma pack()


