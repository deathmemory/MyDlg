#include "CFileIO.hpp"
#include "../../BaseCode/VM_MACRO//VMP.H"

namespace FILEIO
{

	VOID CloseHandleExt(BYTE * pHandle)
	{
		GlobalFree(pHandle);
	}
	BOOL SaveBuffToFile(WCHAR *pfilepath,BYTE *in,DWORD inSize)
	{
		DWORD rd=0;
		//DWORD fileSize=0;
		HANDLE hFile=0;
        VMBEGIN
			hFile = CreateFileW(pfilepath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 	return FALSE;
		if(!WriteFile(hFile,in,inSize,&rd,0))
		{
			CloseHandle(hFile);
			return FALSE;
		}
		CloseHandle(hFile);
		VMEND
		return TRUE;
		
	}
	BOOL SaveBuffToFile(CHAR *pfilepath,BYTE *in,DWORD inSize)
	{
		DWORD rd=0;
		//DWORD fileSize=0;
		HANDLE hFile=0;
		VMBEGIN
		hFile = CreateFileA(pfilepath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 	return FALSE;
		if(!WriteFile(hFile,in,inSize,&rd,0))
		{
			GetLastError();
			CloseHandle(hFile);
			return FALSE;
		}
		CloseHandle(hFile);
		VMEND
		return TRUE;
		
	}
	//MAPPING_FILE* MappingFileToBuff(CHAR *file,DWORD *filesize,DWORD dwShareMode=FILE_SHARE_READ)
	//{
 //       DWORD openMode=0,mapMode=0,viewMode=0;
	//	MAPPING_FILE *mapfile =0;
	//	DWORD fileSize=0;
	//	VMBEGIN								  
 //       if(dwShareMode&FILE_SHARE_READ)
 //       {
 //               openMode |=FILE_SHARE_READ;
 //               mapMode  =PAGE_READONLY;
 //               viewMode |=FILE_MAP_READ;
 //       }
 //       if(dwShareMode&FILE_SHARE_WRITE)
 //       {
 //               openMode |=FILE_SHARE_WRITE;
 //               mapMode =PAGE_READWRITE;
 //               viewMode |=FILE_MAP_WRITE ;
 //       }                
	//	mapfile = (MAPPING_FILE*)GlobalAlloc(GPTR,sizeof(MAPPING_FILE));
	//	
	//	mapfile->m_hInFile = CreateFileA(file, GENERIC_WRITE | GENERIC_READ, dwShareMode, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
	//	if ( mapfile->m_hInFile == INVALID_HANDLE_VALUE )	
	//	{
	//		GlobalFree((BYTE**)mapfile);
	//		return 0;
	//	}
	//	
	//	fileSize = GetFileSize(mapfile->m_hInFile,0);
	//	
	//	mapfile->m_hInFileMapping = CreateFileMappingA( mapfile->m_hInFile, NULL, mapMode, 0, 0, NULL );
	//	if( !mapfile->m_hInFileMapping )
	//	{									
	//		CloseMappingFile( mapfile );
	//		return 0;
	//	}
	//	
	//	mapfile->pmapview = (BYTE*)MapViewOfFile( mapfile->m_hInFileMapping, viewMode, 0, 0, 0 );
	//	if( !mapfile->pmapview )
	//	{				
	//		CloseMappingFile(mapfile );					
	//		return 0;
	//	}
	//	
	//	if(filesize)*filesize=fileSize;
	//	return mapfile;
	//	VMEND
	//}
   // BOOL SearchAndPlace(CHAR* filepath,BYTE *dSearch,BYTE *dReplace,DWORD ReplaceLen,BOOL replaceAll)
   // {
   //         MAPPING_FILE *out=0;
   //         MAPPING_FILE *Pfile;
   //         DWORD fileSize=0;
   //         BOOL result=FALSE;
			//VMBEGIN
   //         out=MappingFileToBuff(filepath,&fileSize,FILE_SHARE_WRITE | FILE_SHARE_READ);
   //         Pfile = out;
   //         while((DWORD)(Pfile->pmapview-out->pmapview)!=(DWORD)(fileSize-ReplaceLen+1))
   //         {
   //                 if(*Pfile->pmapview==*dSearch)
   //                 {
   //                         if(!memcmp(Pfile->pmapview,dSearch,ReplaceLen))
   //                         {
   //                                 CopyMemory(Pfile->pmapview,dReplace,ReplaceLen);
   //                                 result=TRUE;
   //                                 if(!replaceAll)break;
   //                         }
   //                 }
   //                 Pfile->pmapview++;
   //         }

   //         CloseMappingFile(out);
   //       return result;
		 // VMEND
   // }
	//VOID CloseMappingFile(MAPPING_FILE* in)
	//{
	//	MAPPING_FILE *mapfile = in;
	//	VMBEGIN
	//	if(mapfile->m_hInFileMapping)CloseHandle(mapfile->m_hInFileMapping);
	//	if(mapfile->m_hInFile)CloseHandle(mapfile->m_hInFile);
	//	if(mapfile->pmapview)UnmapViewOfFile(mapfile->pmapview);
	//	VMEND

	//}
	BYTE *ReadFileToBuff(WCHAR *pfilepath,DWORD *rdSize)
	{
		BYTE *pBuff=0;
		HANDLE hFile=0;
		DWORD fileSize=0;
		VMBEGIN
		hFile = CreateFileW(pfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 	return 0;
		fileSize = GetFileSize(hFile, NULL)+1;
		pBuff = (BYTE*)GlobalAlloc(GPTR,fileSize);
		if(!rdSize)
			ReadFile(hFile,pBuff,fileSize-1,(DWORD*)&rdSize,0);
		else
			ReadFile(hFile,pBuff,fileSize-1,rdSize,0);
		CloseHandle(hFile);
		//GlobalFree(pBuff);
		VMEND
		return pBuff;
		
	}
	BYTE *ReadFileToBuff(CHAR *pfilepath,DWORD *rdSize)
	{
		BYTE *pBuff=0;
		HANDLE hFile=0;
		DWORD fileSize=0;
		//OFSTRUCT ofs={0};
		VMBEGIN
		hFile = CreateFileA(pfilepath, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE || FILE_ATTRIBUTE_HIDDEN || FILE_ATTRIBUTE_NORMAL || FILE_ATTRIBUTE_READONLY, NULL);

		if (hFile == INVALID_HANDLE_VALUE) 	return 0;
		fileSize = GetFileSize(hFile, NULL)+1;
		pBuff = (BYTE*)GlobalAlloc(GPTR,fileSize);
		if(!rdSize)
			ReadFile(hFile,pBuff,fileSize-1,(DWORD*)&rdSize,0);
		else
			ReadFile(hFile,pBuff,fileSize-1,rdSize,0);
		CloseHandle(hFile);
		//GlobalFree(pBuff);
		VMEND
		return pBuff;
		
	}
	DWORD LogInfo(TCHAR *pfilepath,BYTE *in,DWORD inSize,BOOL br)
	{
		DWORD rd=0;
		//DWORD fileSize=0;
		HANDLE hFile=0;
		VMBEGIN
			hFile = CreateFile(pfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 	return FALSE;
		SetFilePointer(hFile,0,0,FILE_END);
		if(!WriteFile(hFile,in,inSize,&rd,0))
		{
			GetLastError();
			CloseHandle(hFile);
			return FALSE;
		}
		if(br)WriteFile(hFile,"\x0d\x0a",2,&rd,0);
		CloseHandle(hFile);
		VMEND
		return TRUE;
		
	}
	DWORD GetFileSizeExt(TCHAR *pfilepath)
	{
		HANDLE hFile; // the file handle
		DWORD dwFileSize; // temporary storage for file sizes
		VMBEGIN
		hFile = CreateFile(pfilepath, GENERIC_READ | GENERIC_WRITE, 0, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 	return 0;
		dwFileSize = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		VMEND
		return dwFileSize;
		
	}
};