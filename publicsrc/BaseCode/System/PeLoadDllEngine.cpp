#include "stdafx.h"
#include "PeLoadDllEngine.h"
#define ASSERT(i) if(!i)_asm int 3
JK_PeLoadDllEngine::JK_PeLoadDllEngine()
{
	isLoadOk = FALSE;
	pImageBase = NULL;
	pDllMain = NULL;
}
JK_PeLoadDllEngine::~JK_PeLoadDllEngine()
{
	if(isLoadOk)
	{
		ASSERT(pImageBase != NULL);
		ASSERT(pDllMain   != NULL);
		//脱钩，准备卸载dll
		pDllMain((HINSTANCE)&(pImageBase),DLL_PROCESS_DETACH,0);
		VirtualFree((LPVOID)&pImageBase, 0, MEM_RELEASE);
		isLoadOk=false;
	}
}
BYTE* JK_PeLoadDllEngine::FILELOADER::DecodeFile(BYTE *Src,DWORD decodeKey,DWORD len)//装载文件到映像
{
	QUICK_CRYPT::QuickDecrypt(Src,decodeKey,len);
	return 0;
}
BOOL JK_PeLoadDllEngine::FILELOADER::LoadFile( TCHAR *lpFileName,DWORD decodeKey)//装载文件到映像
{
	m_lpInImageBase = FILEIO::ReadFileToBuff(lpFileName,&fileSize);
	DecodeFile(m_lpInImageBase,decodeKey,fileSize);
	/*
	m_hInFile = CreateFile( lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( m_hInFile == INVALID_HANDLE_VALUE )				   
		return FALSE;
	fileSize = GetFileSize(m_hInFile,0);
	m_hInFileMapping = CreateFileMapping( m_hInFile, NULL, PAGE_READONLY, 0, 0, NULL );
	if( !m_hInFileMapping )
	{									
		CloseHandle( m_hInFile );
		m_hInFile = NULL ; return FALSE;
	}

	m_lpInImageBase = (BYTE*)MapViewOfFile( m_hInFileMapping, FILE_MAP_READ, 0, 0, 0 );
	if( !m_lpInImageBase )
	{									
		CloseHandle( m_hInFileMapping );
		CloseHandle( m_hInFile );
		m_hInFileMapping = m_hInFile = NULL; return FALSE;
	}
	*/
	IMAGE_NT_HEADERS *inh;
	IMAGE_DOS_HEADER *idh;
	*(DWORD*)&idh = *(DWORD*)&m_lpInImageBase;
	*(DWORD*)&inh = *(DWORD*)&m_lpInImageBase+idh->e_lfanew;
	ImageSize = inh->OptionalHeader.SizeOfImage;
	//CloseHandle( m_hInFile );
	return TRUE;
}
BOOL JK_PeLoadDllEngine::FILELOADER::LoadFile( TCHAR * lpFileName )
{
	m_hInFile = CreateFile( lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	if ( m_hInFile == INVALID_HANDLE_VALUE )				   
		return FALSE;
	fileSize = GetFileSize(m_hInFile,0);
	m_hInFileMapping = CreateFileMapping( m_hInFile, NULL, PAGE_READONLY, 0, 0, NULL );
	if( !m_hInFileMapping )
	{									
		CloseHandle( m_hInFile );
		m_hInFile = NULL ; return FALSE;
	}

	m_lpInImageBase = (BYTE*)MapViewOfFile( m_hInFileMapping, FILE_MAP_READ, 0, 0, 0 );
	if( !m_lpInImageBase )
	{									
		CloseHandle( m_hInFileMapping );
		CloseHandle( m_hInFile );
		m_hInFileMapping = m_hInFile = NULL; return FALSE;
	}

	IMAGE_NT_HEADERS *inh;
	IMAGE_DOS_HEADER *idh;
	*(DWORD*)&idh = *(DWORD*)&m_lpInImageBase;
	*(DWORD*)&inh = *(DWORD*)&m_lpInImageBase+idh->e_lfanew;
	ImageSize = inh->OptionalHeader.SizeOfImage;
	CloseHandle( m_hInFile );
	return TRUE;
}
void JK_PeLoadDllEngine::FILELOADER::FreeFile()
{
	FILEIO::CloseHandleExt(m_lpInImageBase);
}
void JK_PeLoadDllEngine::FILELOADER::UnLoadFile( void )
{
	UnmapViewOfFile( m_lpInImageBase );
	CloseHandle( m_hInFileMapping );

}
void	JK_PeLoadDllEngine::MemFreeLibrary()
{
	if(isLoadOk)
	{
		ASSERT(pImageBase != NULL);
		ASSERT(pDllMain   != NULL);
		//脱钩，准备卸载dll
		pDllMain((HINSTANCE)&(pImageBase),DLL_PROCESS_DETACH,0);
		VirtualFree((LPVOID)&pImageBase, 0, MEM_RELEASE);
		isLoadOk=false;
	}
	return;
}
//MemLoadLibrary函数从内存缓冲区数据中加载一个dll到当前进程的地址空间，缺省位置0x10000000
//返回值： 成功返回TRUE , 失败返回FALSE
//lpFileData: 存放dll文件数据的缓冲区
//DataLength: 缓冲区中数据的总长度
BOOL JK_PeLoadDllEngine::MemLoadLibrary(TCHAR *filename)
{
	FILELOADER loadDll;
	void* lpFileData;
	int DataLength;
	loadDll.LoadFile(filename);
	//char *DllNameBuffer = "../testDll/Release/netTest.exe";


	lpFileData = loadDll.m_lpInImageBase;
	DataLength = loadDll.fileSize;


	if(pImageBase != NULL)
	{
		return FALSE;  //已经加载一个dll，还没有释放，不能加载新的dll
	}
	//检查数据有效性，并初始化
	if(!CheckDataValide(lpFileData, DataLength))return FALSE;
	//计算所需的加载空间
	int ImageSize = CalcTotalImageSize();
	if(ImageSize == 0) return FALSE;

	// 分配虚拟内存
	void *pMemoryAddress = VirtualAlloc((LPVOID)0, ImageSize,MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
	if(pMemoryAddress == NULL) return FALSE;
	else
	{
		CopyDllDatas(pMemoryAddress, lpFileData); //复制dll数据，并对齐每个段
		//重定位信息
		if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress >0
			&& pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size>0)
		{
			DoRelocation(pMemoryAddress);
		}
		//填充引入地址表
		if(!FillRavAddress(pMemoryAddress)) //修正引入地址表失败
		{
			VirtualFree(pMemoryAddress,0,MEM_RELEASE);
			return FALSE;
		}
		//修改页属性。应该根据每个页的属性单独设置其对应内存页的属性。这里简化一下。
		//统一设置成一个属性PAGE_EXECUTE_READWRITE
		unsigned long old;
		VirtualProtect(pMemoryAddress, ImageSize, PAGE_EXECUTE_READWRITE,&old);
	}
	//修正基地址
	pNTHeader->OptionalHeader.ImageBase = *(DWORD*)&pMemoryAddress;

	//接下来要调用一下dll的入口函数，做初始化工作。
	*(DWORD*)(&pDllMain) = (pNTHeader->OptionalHeader.AddressOfEntryPoint +*(DWORD*)&pMemoryAddress);

	BOOL InitResult = pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_ATTACH,0);
	if(!InitResult) //初始化失败
	{
		pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_DETACH,0);
		VirtualFree(pMemoryAddress,0,MEM_RELEASE);
		pDllMain = NULL;
		return FALSE;
	}

	isLoadOk = TRUE;
	pImageBase = *(DWORD*)&pMemoryAddress;
	loadDll.UnLoadFile();
	return TRUE;
}
BOOL JK_PeLoadDllEngine::MemLoadLibrary(TCHAR *filename,DWORD decodeKey)
{
	FILELOADER loadDll;
	void* lpFileData;
	DWORD old;
	int DataLength;
	int ImageSize;
	BOOL InitResult;
	loadDll.LoadFile(filename,decodeKey);
	//char *DllNameBuffer = "../testDll/Release/netTest.exe";

	
	lpFileData = loadDll.m_lpInImageBase;
	DataLength = loadDll.fileSize;


	if(pImageBase != NULL)
	{
		return FALSE;  //已经加载一个dll，还没有释放，不能加载新的dll
	}
	//检查数据有效性，并初始化
	if(!CheckDataValide(lpFileData, DataLength))return FALSE;
	//计算所需的加载空间
	ImageSize = CalcTotalImageSize();
	if(ImageSize == 0) return FALSE;

	// 分配虚拟内存
	void *pMemoryAddress = VirtualAlloc((LPVOID)0, ImageSize,MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE); 
	if(pMemoryAddress == NULL) return FALSE;
	else
	{
		CopyDllDatas(pMemoryAddress, lpFileData); //复制dll数据，并对齐每个段
		//重定位信息
		if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress >0
			&& pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size>0)
		{
			DoRelocation(pMemoryAddress);
		}
		//填充引入地址表
		if(!FillRavAddress(pMemoryAddress)) //修正引入地址表失败
		{
			VirtualFree(pMemoryAddress,0,MEM_RELEASE);
			return FALSE;
		}
		//修改页属性。应该根据每个页的属性单独设置其对应内存页的属性。这里简化一下。
		//统一设置成一个属性PAGE_EXECUTE_READWRITE
		
		VirtualProtect(pMemoryAddress, ImageSize, PAGE_EXECUTE_READWRITE,&old);
	}
	//修正基地址
	pNTHeader->OptionalHeader.ImageBase = *(DWORD*)&pMemoryAddress;

	//接下来要调用一下dll的入口函数，做初始化工作。
	*(DWORD*)(&pDllMain) = (pNTHeader->OptionalHeader.AddressOfEntryPoint +*(DWORD*)&pMemoryAddress);
	
	InitResult = pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_ATTACH,0);
	if(!InitResult) //初始化失败
	{
		pDllMain((HINSTANCE)pMemoryAddress,DLL_PROCESS_DETACH,0);
		VirtualFree(pMemoryAddress,0,MEM_RELEASE);
		pDllMain = NULL;
		return FALSE;
	}

	isLoadOk = TRUE;
	pImageBase = *(DWORD*)&pMemoryAddress;
	loadDll.FreeFile();
	return TRUE;
}
/////
////////获得导出函数相对于映像地址的偏移
//////
DWORD JK_PeLoadDllEngine::MemOffsetProcAddr(CHAR *lpProcName)
{
	DWORD offset;
	offset = (DWORD)MemGetProcAddress(lpProcName);
	offset = offset - pNTHeader->OptionalHeader.ImageBase;
	return offset;

}
//MemGetProcAddress函数从dll中获取指定函数的地址
//返回值： 成功返回函数地址 , 失败返回NULL
//lpProcName: 要查找函数的名字或者序号
FARPROC  JK_PeLoadDllEngine::MemGetProcAddress(CHAR *lpProcName)
{
	CHAR* pName=0;
	LPWORD  pAddressOfOrdinals =0;
	LPDWORD pAddressOfNames  = 0,pAddressOfFunctions=0;
	PIMAGE_EXPORT_DIRECTORY pExport=0;
	DWORD OffsetStart,Size,pFunctionOffset;
	FARPROC retfunc;
	int iBase,iNumberOfFunctions ,iNumberOfNames,iOrdinal = -1,iFound = -1;

	if(pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0 ||
		pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0)
		return NULL;
	if(!isLoadOk) return NULL;

	OffsetStart = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	Size = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	
	*(DWORD*)(&pExport) = ((DWORD)pImageBase + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	iBase = pExport->Base;
	iNumberOfFunctions = pExport->NumberOfFunctions;
	iNumberOfNames = pExport->NumberOfNames; //<= iNumberOfFunctions


	*(DWORD*)(& pAddressOfFunctions) = (pExport->AddressOfFunctions + pImageBase);
	*(DWORD*)(&pAddressOfOrdinals) = (pExport->AddressOfNameOrdinals + pImageBase);
	*(DWORD*)(& pAddressOfNames)  = (pExport->AddressOfNames + pImageBase);

	

	if(((*(DWORD*)(&lpProcName)) & 0xFFFF0000) == 0) //IT IS A ORDINAL!
	{
		iOrdinal = (*(DWORD*)(&lpProcName)) & 0x0000FFFF - iBase;
	}
	else  //use name
	{
		for(int i=0;i<iNumberOfNames;i++)
		{
			
			*(DWORD*)(&pName)= (pAddressOfNames[i] + pImageBase);
			if(lstrcmpA(pName, lpProcName) == 0)
			{
				iFound = i; break;
			}
		}
		if(iFound >= 0)
		{
			iOrdinal = (int)(pAddressOfOrdinals[iFound]);
		}
	}

	if(iOrdinal < 0 || iOrdinal >= iNumberOfFunctions ) return NULL;
	else
	{
		pFunctionOffset = pAddressOfFunctions[iOrdinal];
		if(pFunctionOffset > OffsetStart && pFunctionOffset < (OffsetStart+Size))//maybe Export Forwarding
			return NULL;
		else 
		{
			
			*(DWORD*)(&retfunc)=(pFunctionOffset + pImageBase);
			return retfunc;
		}
	}

}


// 重定向PE用到的地址
void JK_PeLoadDllEngine::DoRelocation( void *NewBase)
{
	/* 重定位表的结构：
	// DWORD sectionAddress, DWORD size (包括本节需要重定位的数据)
	// 例如 1000节需要修正5个重定位数据的话，重定位表的数据是
	// 00 10 00 00   14 00 00 00      xxxx xxxx xxxx xxxx xxxx 0000
	// -----------   -----------      ----
	// 给出节的偏移  总尺寸=8+6*2     需要修正的地址           用于对齐4字节
	// 重定位表是若干个相连，如果address 和 size都是0 表示结束
	// 需要修正的地址是12位的，高4位是形态字，intel cpu下是3
	*/
	//假设NewBase是0x600000,而文件中设置的缺省ImageBase是0x400000,则修正偏移量就是0x200000
	DWORD Delta = (DWORD)(*(DWORD*)&NewBase - pNTHeader->OptionalHeader.ImageBase);

	//注意重定位表的位置可能和硬盘文件中的偏移地址不同，应该使用加载后的地址
	PIMAGE_BASE_RELOCATION pLoc =0;
	*(DWORD*)(&pLoc)= (*(DWORD*)&NewBase + pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	while((pLoc->VirtualAddress + pLoc->SizeOfBlock) != 0) //开始扫描重定位表
	{
		WORD *pLocData = 0;
		*(int*)(&pLocData)=(*(int*)&pLoc + (int)sizeof(IMAGE_BASE_RELOCATION));
		//计算本节需要修正的重定位项（地址）的数目
		int NumberOfReloc = (pLoc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION))/sizeof(WORD);
		for( int i=0 ; i < NumberOfReloc; i++)
		{
			if( (DWORD)(pLocData[i] & 0xF000) == 0x00003000) //这是一个需要修正的地址
			{
				// 举例： 
				// pLoc->VirtualAddress = 0x1000; 
				// pLocData[i] = 0x313E; 表示本节偏移地址0x13E处需要修正
				// 因此 pAddress = 基地址 + 0x113E
				// 里面的内容是 A1 ( 0c d4 02 10)  汇编代码是： mov eax , [1002d40c]
				// 需要修正1002d40c这个地址
				DWORD * pAddress = 0;
				*(DWORD*)&pAddress = (*(DWORD*)&NewBase + pLoc->VirtualAddress + (pLocData[i] & 0x0FFF));
				*pAddress += Delta;
			}
		}
		//转移到下一个节进行处理
		*(DWORD*)(&pLoc)=(*(DWORD*)&pLoc + pLoc->SizeOfBlock);
	}
}
//更改引入地址表
BOOL ChangeRavAddress(void* pImageBase,CHAR *funcName,CHAR *dllname,DWORD newAddr)
{
	/*
	int i=0;
	// 引入表实际上是一个 IMAGE_IMPORT_DESCRIPTOR 结构数组，全部是0表示结束
	// 数组定义如下：
	// 
	// DWORD   OriginalFirstThunk;         // 0表示结束，否则指向未绑定的IAT结构数组
	// DWORD   TimeDateStamp; 
	// DWORD   ForwarderChain;             // -1 if no forwarders
	// DWORD   Name;                       // 给出dll的名字
	// DWORD   FirstThunk;                 // 指向IAT结构数组的地址(绑定后，这些IAT里面就是实际的函数地址)
	PIMAGE_NT_HEADERS lpNTHeader;
	PIMAGE_DOS_HEADER lpDosHeader;
	PIMAGE_SECTION_HEADER lpSectionHeader;
	lpDosHeader = (PIMAGE_DOS_HEADER)pImageBase;
	//新的pe头地址
	*(DWORD*)&lpNTHeader = (*(DWORD*)&pImageBase + (lpDosHeader->e_lfanew));
	//新的节表地址
	*(DWORD*)&lpSectionHeader = (*(DWORD*)&lpNTHeader + sizeof(IMAGE_NT_HEADERS));


	DWORD Offset = lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ;
	if(Offset == 0) return TRUE; //No Import Table
	PIMAGE_IMPORT_DESCRIPTOR pID =0;
	*(DWORD*)(&pID)= (*(DWORD*) &pImageBase + Offset);
	//while(pID->Characteristics != 0)
		//int cnt=0;
	for(int cnt=0;cnt<lpNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size/sizeof(PIMAGE_IMPORT_DESCRIPTOR);cnt++)
	{
		PIMAGE_THUNK_DATA pRealIAT =0;
		*(DWORD*)(&pRealIAT)=(*(DWORD*)&pImageBase + pID->FirstThunk);
		PIMAGE_THUNK_DATA pOriginalIAT =0;
		*(DWORD*)(&pOriginalIAT) = (*(DWORD*)&pImageBase + pID->OriginalFirstThunk);
		//获取dll的名字
		CHAR buf[MAX_PATH]; //dll name;
		BYTE* pName = 0;
		*(DWORD*)(&pName)=(*(DWORD*)&pImageBase + pID->Name);
		for(i=0;i<sizeof(buf);i++)
		{
			if(pName[i] == 0)break;
			buf[i] = pName[i];
		}
		if(i>=sizeof(buf)) return FALSE;  // bad dll name
		else buf[i] = 0;
	
		//if(lstrcmpiA(dllname,buf))
		//{
		//	*(DWORD*)&pID = ( *(DWORD*)&pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
		//	continue;
		//}

		
		HMODULE hDll = GetModuleHandleA(buf); 
		if(hDll == NULL) { 
			hDll = LoadLibraryA(buf); 
			if (hDll == NULL) 
				return FALSE; //NOT FOUND DLL 
		} 
		
		for(i=0; ;i++)
		{
			if(pOriginalIAT[i].u1.Function == 0)
				break;
			FARPROC lpFunction = NULL;
			if(pOriginalIAT[i].u1.Ordinal & IMAGE_ORDINAL_FLAG) //这里的值给出的是导出序号
			{
				lpFunction = GetProcAddress((HMODULE)hDll, (CHAR*)(pOriginalIAT[i].u1.Ordinal & 0x0000FFFF));
			}
			else //按照名字导入
			{
				//获取此IAT项所描述的函数名称
				PIMAGE_IMPORT_BY_NAME pByName = 0;
				*(DWORD*)(&pByName) = (*(DWORD*)&pImageBase + (DWORD)(pOriginalIAT[i].u1.AddressOfData));
				//    if(pByName->Hint !=0)
				//     lpFunction = GetProcAddress(hDll, (LPCSTR)pByName->Hint);
				//    else
				lpFunction = GetProcAddress((HMODULE)hDll, (char *)pByName->Name);
			}
			if(lpFunction != NULL)   //找到了！
			{
				//pRealIAT[i].u1.Function = *(DWORD*)&lpFunction;
				__asm MOV EAX,lpFunction
			}
			else return FALSE;
		}

		//move to next 
		*(DWORD*)&pID = ( *(DWORD*)&pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	*/
	return TRUE;
}
//填充引入地址表
BOOL JK_PeLoadDllEngine::FillRavAddress(void *pImageBase)
{
	int i=0;
	// 引入表实际上是一个 IMAGE_IMPORT_DESCRIPTOR 结构数组，全部是0表示结束
	// 数组定义如下：
	// 
	// DWORD   OriginalFirstThunk;         // 0表示结束，否则指向未绑定的IAT结构数组
	// DWORD   TimeDateStamp; 
	// DWORD   ForwarderChain;             // -1 if no forwarders
	// DWORD   Name;                       // 给出dll的名字
	// DWORD   FirstThunk;                 // 指向IAT结构数组的地址(绑定后，这些IAT里面就是实际的函数地址)
	unsigned long Offset = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress ;
	if(Offset == 0) return TRUE; //No Import Table
	PIMAGE_IMPORT_DESCRIPTOR pID =0;
	*(DWORD*)(&pID)= (*(DWORD*) &pImageBase + Offset);
	while(pID->Characteristics != 0)
		//int cnt=0;
		//for(cnt=0;cnt<pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size/sizeof(PIMAGE_IMPORT_DESCRIPTOR);cnt++)
	{
		PIMAGE_THUNK_DATA pRealIAT =0;
		*(DWORD*)(&pRealIAT)=(*(DWORD*)&pImageBase + pID->FirstThunk);
		PIMAGE_THUNK_DATA pOriginalIAT =0;
		*(DWORD*)(&pOriginalIAT) = (*(DWORD*)&pImageBase + pID->OriginalFirstThunk);
		//获取dll的名字
		CHAR buf[MAX_PATH]; //dll name;
		BYTE* pName = 0;
		*(DWORD*)(&pName)=(*(DWORD*)&pImageBase + pID->Name);
		for(i=0;i<sizeof(buf);i++)
		{
			if(pName[i] == 0)break;
			buf[i] = pName[i];
		}
		if(i>=sizeof(buf)) return FALSE;  // bad dll name
		else buf[i] = 0;

		HMODULE hDll = GetModuleHandleA(buf); 
		if(hDll == NULL) { 
			hDll = LoadLibraryA(buf); 
			if (hDll == NULL) 
				return FALSE; //NOT FOUND DLL 
		} 
		//获取DLL中每个导出函数的地址，填入IAT
		//每个IAT结构是 ：
		// union { PBYTE  ForwarderString;
		//   PDWORD Function;
		//   DWORD Ordinal;
		//   PIMAGE_IMPORT_BY_NAME  AddressOfData;
		// } u1;
		// 长度是一个DWORD ，正好容纳一个地址。
		for(i=0; ;i++)
		{
			if(pOriginalIAT[i].u1.Function == 0)
				break;
			FARPROC lpFunction = NULL;
			if(pOriginalIAT[i].u1.Ordinal & IMAGE_ORDINAL_FLAG) //这里的值给出的是导出序号
			{
				lpFunction = GetProcAddress(hDll, (CHAR*)(pOriginalIAT[i].u1.Ordinal & 0x0000FFFF));
			}
			else //按照名字导入
			{
				//获取此IAT项所描述的函数名称
				PIMAGE_IMPORT_BY_NAME pByName = 0;
				*(DWORD*)(&pByName) = (*(DWORD*)&pImageBase + (DWORD)(pOriginalIAT[i].u1.AddressOfData));
				//    if(pByName->Hint !=0)
				//     lpFunction = GetProcAddress(hDll, (LPCSTR)pByName->Hint);
				//    else
				lpFunction = GetProcAddress(hDll, (char *)pByName->Name);
			}
			if(lpFunction != NULL)   //找到了！
			{
				pRealIAT[i].u1.Function = *(DWORD*)&lpFunction;
			}
			else return FALSE;
		}

		//move to next 
		*(DWORD*)&pID = ( *(DWORD*)&pID + sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
	return TRUE;
}

//CheckDataValide函数用于检查缓冲区中的数据是否有效的dll文件
//返回值： 是一个可执行的dll则返回TRUE，否则返回FALSE。
//lpFileData: 存放dll数据的内存缓冲区
//DataLength: dll文件的长度
BOOL JK_PeLoadDllEngine::CheckDataValide(void* lpFileData, int DataLength)
{
	//检查长度
	if(DataLength < sizeof(IMAGE_DOS_HEADER)) return FALSE;
	pDosHeader = (PIMAGE_DOS_HEADER)lpFileData;  // DOS头
	//检查dos头的标记
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) return FALSE;  //0x5A4D : MZ

	//检查长度
	if((DWORD)DataLength < (pDosHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS)) ) return FALSE;
	//取得pe头
	*(DWORD*)&pNTHeader = ( *(DWORD*)&lpFileData + pDosHeader->e_lfanew); // PE头
	//检查pe头的合法性
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE) return FALSE;  //0x00004550 : PE00
	if((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) //0x2000  : File is a DLL
		return FALSE;  
	if((pNTHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE) == 0) //0x0002 : 指出文件可以运行
		return FALSE;
	if(pNTHeader->FileHeader.SizeOfOptionalHeader != sizeof(IMAGE_OPTIONAL_HEADER)) return FALSE;


	//取得节表（段表）
	*(DWORD*)&pSectionHeader = (*(DWORD*)&pNTHeader + sizeof(IMAGE_NT_HEADERS));
	//验证每个节表的空间
	for(int i=0; i< pNTHeader->FileHeader.NumberOfSections; i++)
	{
		if((pSectionHeader[i].PointerToRawData + pSectionHeader[i].SizeOfRawData) > (DWORD)DataLength)return FALSE;
	}
	return TRUE;
}

//计算对齐边界
int JK_PeLoadDllEngine::GetAlignedSize(int Origin, int Alignment)
{
	return (Origin + Alignment - 1) / Alignment * Alignment;
}
//计算整个dll映像文件的尺寸
int JK_PeLoadDllEngine::CalcTotalImageSize()
{
	int Size;
	if(pNTHeader == NULL)return 0;
	int nAlign = pNTHeader->OptionalHeader.SectionAlignment; //段对齐字节数

	// 计算所有头的尺寸。包括dos, coff, pe头 和 段表的大小
	Size = GetAlignedSize(pNTHeader->OptionalHeader.SizeOfHeaders, nAlign);
	// 计算所有节的大小
	for(int i=0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
	{
		//得到该节的大小
		int CodeSize = pSectionHeader[i].Misc.VirtualSize ;
		int LoadSize = pSectionHeader[i].SizeOfRawData;
		int MaxSize = (LoadSize > CodeSize)?(LoadSize):(CodeSize);

		int SectionSize = GetAlignedSize(pSectionHeader[i].VirtualAddress + MaxSize, nAlign);
		if(Size < SectionSize) 
			Size = SectionSize;  //Use the Max;
	}
	return Size;
}
//CopyDllDatas函数将dll数据复制到指定内存区域，并对齐所有节
//pSrc: 存放dll数据的原始缓冲区
//pDest:目标内存地址
void JK_PeLoadDllEngine::CopyDllDatas(void* pDest, void* pSrc)
{
	// 计算需要复制的PE头+段表字节数
	int  HeaderSize = pNTHeader->OptionalHeader.SizeOfHeaders;
	int  SectionSize = pNTHeader->FileHeader.NumberOfSections * sizeof(IMAGE_SECTION_HEADER);
	int  MoveSize = HeaderSize + SectionSize;
	//复制头和段信息
	memmove(pDest, pSrc, MoveSize);

	//复制每个节
	for(int i=0; i < pNTHeader->FileHeader.NumberOfSections; ++i)
	{
		if(pSectionHeader[i].VirtualAddress == 0 || pSectionHeader[i].SizeOfRawData == 0)continue;
		// 定位该节在内存中的位置
		void *pSectionAddress =0;
		*(DWORD*)&pSectionAddress=(*(DWORD*)&pDest + pSectionHeader[i].VirtualAddress);
		// 复制段数据到虚拟内存
		memmove((void *)pSectionAddress,(void *)(*(DWORD*)&pSrc + pSectionHeader[i].PointerToRawData),pSectionHeader[i].SizeOfRawData);
	}

	//修正指针，指向新分配的内存
	//新的dos头
	pDosHeader = (PIMAGE_DOS_HEADER)pDest;
	//新的pe头地址
	*(DWORD*)&pNTHeader = (*(DWORD*)&pDest + (pDosHeader->e_lfanew));
	//新的节表地址
	*(DWORD*)&pSectionHeader = (*(DWORD*)&pNTHeader + sizeof(IMAGE_NT_HEADERS));
	return ;
}
