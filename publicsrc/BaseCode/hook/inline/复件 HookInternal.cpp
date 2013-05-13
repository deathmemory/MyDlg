#include "hookInternal.h"
#include "../../String/STRING_EX.h"
#include "../../System/System.hpp"
using namespace SYSTEM_INTERFACE;
#include "../../dasm/disassemble.cpp"
#pragma pack(1)

#define WRITE_PAGE_SIZE 100
BYTE opSize[MAX_PATH][MAX_PATH]={NULL};

#define ANY_CODE			0x2A2A	//** 作为一个字节的通配符号
static BYTE HookCode=0xE9;
HOOK_INTERNAL::tagHOOK_INTERNAL()
{
	ZeroMemory(pHookAddrCode,sizeof(pHookAddrCode));
}
DWORD HOOK_INTERNAL::SearchAndPlace(BYTE *imageBase,BYTE *dSearch,BYTE *dReplace,DWORD ReplaceLen,BOOL replaceAll)
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
	return result;
	VMEND
}
void HOOK_INTERNAL::ReadRestoreCodeFromFile(TCHAR *file)
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



void HOOK_INTERNAL::HookAddress()
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
		SYSTEM_INTERFACE::_VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);

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
		SYSTEM_INTERFACE::_WriteProcessMemory(hProcess,xhookAddr,&HookCode,1,&rwS);
		SYSTEM_INTERFACE::_WriteProcessMemory(hProcess,(BYTE*)((BYTE*)(xhookAddr)+1),&NakedFunc,4,&rwS);


		SYSTEM_INTERFACE::_VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}else
	{
		SYSTEM_INTERFACE::_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);

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

		SYSTEM_INTERFACE::_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}

	return;
	VMEND

}
void HOOK_INTERNAL::UnhookAddress(BOOL fromFile)
{
	if(!this->pHookAddr)return;
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
		SYSTEM_INTERFACE::_VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);
		SYSTEM_INTERFACE::_WriteProcessMemory(hProcess,xhookAddr,RestoreCode,8,&oldp);
		SYSTEM_INTERFACE::_VirtualProtectEx(hProcess,(void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}
	else
	{
		SYSTEM_INTERFACE::_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);
		__asm
		{

			MOV ESI,RestoreCode
			MOV EDI,xhookAddr
			MOV EAX,[ESI]
			MOV [EDI],EAX
			MOV EAX,[ESI+4]
			MOV [EDI+4],EAX
		}

		SYSTEM_INTERFACE::_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
	}
	VMEND
}
void HOOK_INTERNAL::InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,char *function,DWORD funOffset,void *newFun,BYTE *restorCode,int len,BOOL runRestoreCode)
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
void HOOK_INTERNAL::InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,char *function,void *newFun,BOOL runRestoreCode)
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
VOID HOOK_INTERNAL::InitRestore()
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

BOOL HOOK_INTERNAL::InitHookKeyCode(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,DWORD *newFunc,char* findCode,BOOL runRestoreCode)
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
void HOOK_INTERNAL::InitHook(HANDLE _hProcess,DWORD offsetbyte,TCHAR *module,void *HookBaseAddr,void *newFun,BOOL runRestoreCode)
{
	bRunRestoreCode = runRestoreCode;
	hProcess = _hProcess;
	OffsetByte = offsetbyte;
	MEMORY_BASIC_INFORMATION mbi={NULL};
	_VirtualQuery((BYTE*)HookBaseAddr,&mbi,sizeof(MEMORY_BASIC_INFORMATION));
	ModuleBase=(HINSTANCE)mbi.AllocationBase;
	pHookAddr = (DWORD*)HookBaseAddr;
	pCallBack = (DWORD*)newFun;
	ReadRestoreCodeFromFile(module);
	InitRestore();
	HookAddress();
	
}

DWORD *HOOK_INTERNAL::GetFunRealAddr(DWORD *funaddr)
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

DWORD HOOK_INTERNAL::SearchKeyCode(DWORD Pid,DWORD searchFrom,char *KeyCode,DWORD searchSize)
{
	
	DWORD ret=0;
	char *cmpcode=0;
	HANDLE hProcess=_OpenProcess(PROCESS_ALL_ACCESS,FALSE,Pid);
	if(!searchSize)searchSize=0x0080000;

	
	
	if(Pid==GetCurrentProcessId())
	{
		cmpcode = (char*)searchFrom;
		ret = SearchKeyStr(GetCurrentProcess(),(BYTE*)cmpcode,KeyCode,lstrlenA(KeyCode),searchSize-4);
	}else
	{
		cmpcode=(char*)GlobalAlloc(GPTR,searchSize);
		_ReadProcessMemory(hProcess,(LPCVOID)searchFrom,cmpcode,searchSize,0);
		ret = SearchKeyStr(hProcess,(BYTE*)cmpcode,KeyCode,lstrlenA(KeyCode),searchSize-4);
		GlobalFree(cmpcode);
	}

	CloseHandle(hProcess);
	return ret;
}
ULONG HOOK_INTERNAL::ConvertHexToDec(CHAR* szHex, BYTE nHexLen)
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
BOOL WINAPI EnumVirtualMemoryEx(HANDLE hp,DWORD beginaddr,MEMORY_BASIC_INFORMATION *t)
{  
	//HANDLE hp=0;
	VOID * pAddress=(VOID*)beginaddr;  
	//char path[MAX_PATH*2]={0};  
	//char viewstr[MAX_PATH]={0};
	DWORD dwResult=0;  
	//BOOL isExecute=FALSE;
	//DWORD row=0;
	//pid = GetCurrentProcessId();
	//HANDLE hp = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);
	//hp = GetCurrentProcess();
	//if(!hp)return FALSE;
	do
	{
		dwResult=VirtualQueryEx(hp,pAddress,t,sizeof(MEMORY_BASIC_INFORMATION));  
		/*
		if((DWORD)pAddress<0x401000)
		{
			pAddress=(PBYTE)pAddress+t->RegionSize;
			continue;
		}
		*/
		/*
		switch(t->State)
		{  
		case MEM_COMMIT:  
			//	lstrcat(viewstr,"状态：已经物理分配");
			break;  
		case MEM_RESERVE:  
			//lstrcat(viewstr,"状态：保留");
			break;  
		case MEM_FREE:  
			//lstrcat(viewstr,"状态：空闲没有分配");
			break;  
		default:  
			break;  
		}
		*/
		switch(t->Protect)
		{
		case PAGE_NOACCESS:
		case PAGE_GUARD:
		case PAGE_NOCACHE:
		case PAGE_WRITECOMBINE:
		case PAGE_WRITECOPY:
		case PAGE_READONLY:
		case PAGE_READWRITE:
			break;

		case PAGE_EXECUTE:
		case PAGE_EXECUTE_READ:
		case PAGE_EXECUTE_READWRITE:
		case PAGE_EXECUTE_WRITECOPY:
			{

			
			//CloseHandle(hp);
			return TRUE;
			}
		}
		/*
		switch(t->Type)  
		{  
		case MEM_IMAGE:  

			break;  
		case MEM_MAPPED:  

			break;  
		case MEM_PRIVATE:  

			break;  
		default:  
			break;  
		}  
		*/
		pAddress=(PBYTE)pAddress+t->RegionSize;  

	}while(dwResult);
	//CloseHandle(hp);
	return FALSE;
}  
DWORD HOOK_INTERNAL::SearchKeyStr(HANDLE hp,BYTE* pbegin,CHAR *fcode,ULONG lenCode,ULONG searchLen)
{
	DWORD	cntSearch=0,cntCmp=0;
	CHAR	*ptr;
	BYTE	*pCode;
	BOOL isgo = FALSE;
	DWORD rlen = lenCode/2;
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
	
	do 
	{
		isgo = EnumVirtualMemoryEx(hp,(DWORD)pCode,&t);
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
				//if((searchLen - cntSearch)<rlen)break;
				if(*(WORD*)((BYTE*)(ptr)+cntCmp)==ANY_CODE){continue;}
				if(pCode[cntSearch+(cntCmp>>1)]==hexcode[cntCmp>>1])//(BYTE)ConvertHexToDec((CHAR*)(ptr+cntCmp),1))
				{
					if(cntCmp+2==lenCode)
					{
						free(hexcode);
						return (DWORD)(BYTE*)(pCode+cntSearch);
					}
				}else{break;}

			}
		}
		pCode = (BYTE*)((DWORD)(t.BaseAddress)+t.RegionSize);
	} while (isgo);
	free(hexcode);
	return 0;
}

void HOOK_INTERNAL::HookAddressCurrentProcess()
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
		_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);

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

		_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);

	VMEND
	return;
	

}
void HOOK_INTERNAL::UnhookAddressCurrentProcess(BOOL fromFile)
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

	_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,PAGE_EXECUTE_READWRITE,&oldp);
	__asm
	{

		MOV ESI,RestoreCode
		MOV EDI,xhookAddr
		MOV EAX,[ESI]
		MOV [EDI],EAX
		MOV EAX,[ESI+4]
		MOV [EDI+4],EAX
	}

	_VirtualProtect((void*)xhookAddr,WRITE_PAGE_SIZE,oldp,&temp);
VMEND

}
DWORD HOOK_INTERNAL::InitHookCurrentProcess(DWORD offsetbyte,VOID *HookBaseAddr,VOID *newFun,BOOL runRestoreCode)
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
DWORD HOOK_INTERNAL::InitHookCurrentProcess(DWORD offsetbyte,TCHAR *module,CHAR *funname,VOID *newFun,BOOL runRestoreCode)
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
		_VirtualProtect(JmpBack,sizeof(JmpBack),PAGE_EXECUTE_READWRITE,&olptr);
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


