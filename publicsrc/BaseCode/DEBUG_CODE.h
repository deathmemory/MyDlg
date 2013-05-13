#pragma once
#include "./File/CFileIO.hpp"
#ifdef _DEBUG
#define ERRBOX(str) MessageBox(0,_T(str),0,0);
#define LOG_VALUE(dword,br){\
CHAR log[MAX_PATH]={0};\
wsprintfA(log,"0x%.8X",dword);\
FILEIO::LogInfo(_T("./logout.log"),(BYTE*)log,lstrlenA(log),br);\
}
#define LOG_STRING(str,br){\
FILEIO::LogInfo(_T("./logout.log"),(BYTE*)str,lstrlenA(str),br);\
}
#else
#define LOG_VALUE(dword,br) __asm nop
#define LOG_STRING(str,br) __asm nop
#define WRITE_LOG(dword) __asm nop
#define ERRBOX(str) __asm nop
#endif