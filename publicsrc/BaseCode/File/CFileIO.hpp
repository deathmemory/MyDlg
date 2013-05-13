#pragma   once 
#define _FILEIO
//#ifndef _IOSTREAM1_
//#define _IOSTREAM1_
//typedef struct tagMAPPING_FILE
//{
//	BYTE *pmapview;
//	HANDLE m_hInFileMapping;
//	HANDLE m_hInFile;
//}MAPPING_FILE;
namespace FILEIO
{

	//extern VOID CloseMappingFile(MAPPING_FILE* in);
	DWORD LogInfo(TCHAR *pfilepath,BYTE *in,DWORD inSize,BOOL br);
	VOID CloseHandleExt(BYTE * pHandle);
	BYTE *ReadFileToBuff(WCHAR *pfilepath,DWORD *rdSize);
	DWORD GetFileSizeExt(TCHAR *pfilepath);
	extern BOOL SaveBuffToFile(WCHAR *pfilepath,BYTE *in,DWORD size);
	extern BOOL SaveBuffToFile(CHAR *pfilepath,BYTE *in,DWORD inSize);
	BYTE *ReadFileToBuff(CHAR *pfilepath,DWORD *rdSize);
	//extern BOOL SearchAndPlace(CHAR* filepath,BYTE *dSearch,BYTE *dReplace,DWORD ReplaceLen,BOOL replaceAll);
	//extern MAPPING_FILE* MappingFileToBuff(CHAR *file,DWORD *filesize,DWORD dwShareMode);

};


//#endif   //µÚÒ»¸öif

