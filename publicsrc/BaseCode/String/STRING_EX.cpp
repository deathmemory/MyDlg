#ifdef _WINDOWS
    #include "stdafx.h"
#else
    #include <windows.h>
#endif
#include "STRING_EX.h"
namespace STRING_EX
{
	static CHAR *pNextPointer=0;
	static BYTE *pDataBlock=0;
	CHAR *StringToIntString(CHAR *in,DWORD lenstr,CHAR * split)
	{
		BYTE vInt=0;
		CHAR *beginPtr = in;
		if(!beginPtr)return 0;
		int encLen=0;
		if(!lenstr)
			encLen = lstrlenA(in);
		else
			encLen = lenstr;
		CHAR *sIntString= (CHAR*)GlobalAlloc(GPTR,(encLen+1)*4);
		//static char sIntString[MAX_PATH];
		//ZeroMemory(sIntString,(encLen+1)*4);
		while((in - beginPtr)!=encLen)
		{
			vInt = (BYTE)*(in);
			wsprintfA((char*)(sIntString+lstrlenA(sIntString)),"%u%s",vInt,split);
			in++;
		}
		*(CHAR*)(sIntString+lstrlenA(sIntString)-1)=0;
		//free(sIntString);
		return sIntString;
	}
	CHAR *MakeRetValue(CHAR *psrc,DWORD lenstr)
	{
		CHAR *retStr =0;
		//CHAR *P;
		int encLen=0;
		if(!lenstr)
			encLen = lstrlenA(psrc);
		else
			encLen = lenstr;
	//	P = StringToIntString(psrc,lenstr,split);
		retStr= (CHAR*)GlobalAlloc(GPTR,(encLen+1)+MAX_PATH);
		lstrcpyA(retStr,"<string>");
		lstrcatA(retStr,psrc);
		lstrcatA(retStr,"</string>");
		//wsprintfA(retStr,"<string>%s</string>",psrc);

		//FreeString((BYTE*)P);
		return retStr;
	}
	CHAR *GetNextString(CHAR *endFlag)
	{
		CHAR * pbegin=pNextPointer;
		pNextPointer = StrStrIA(pNextPointer,endFlag);
		if(!pNextPointer)return 0;
		*pNextPointer=0;
		pNextPointer++;
		if(!pbegin)return 0;
		return pbegin;
	}

	DWORD GetNextInt(CHAR *endFlag)
	{
		CHAR *ptr = GetNextString(endFlag);
		if(!ptr)return -1;
		return StrToIntA(ptr);
	}

	CHAR *GetFirstString(CHAR *srcXMLString,CHAR *endFlag)
	{
		//CHAR *beginFlag = "<string>",
		CHAR *pbegin=srcXMLString;
		//pbegin=(CHAR*)(StrStrIA(srcXMLString,beginFlag)+lstrlenA(beginFlag));
		if(!pbegin)return 0;
		pNextPointer = StrStrIA(pbegin,endFlag);
		*pNextPointer=0;
		pNextPointer++;
		return pbegin;
	}
	DWORD GetFirstInt(CHAR *srcXMLString,CHAR *endFlag)
	{
		return StrToIntA(GetFirstString(srcXMLString,endFlag));
	}
	BYTE *IntStringToBuffer(CHAR *in,CHAR * split)
	{

		BYTE vInt=0,*siPtr =0, *outBuff=0;;
		//CHAR *beginPtr = in;
		DWORD encLen=0;
		encLen = lstrlenA(in);

		siPtr = outBuff= (BYTE*)GlobalAlloc(GPTR,(encLen+1));
		vInt = (BYTE)GetFirstInt(in,split);
		do
		{
			*(BYTE*)(siPtr) = vInt;
			vInt = (BYTE)GetNextInt(split);
			siPtr++;
		}while((DWORD)(pNextPointer-in)<encLen);
		//*(CHAR*)(sIntString+lstrlenA(sIntString)-1)=0;
		//free(sIntString);
		return outBuff;
	}
	CHAR *IntStringToString(CHAR *in,DWORD lenstr,CHAR * split)
	{

		BYTE vInt=0;
		//CHAR *beginPtr = in;
		int encLen=0;
		if(!lenstr)
			encLen = lstrlenA(in);
		else
			encLen = lenstr;
		CHAR *siPtr =0, *sIntString=0;
		siPtr = sIntString= (CHAR*)GlobalAlloc(GPTR,(encLen+1));
		//static char sIntString[MAX_PATH];
		//ZeroMemory(sIntString,(encLen+1)*2);
		vInt = (CHAR)GetFirstInt(in,split);
		while(vInt!=0xFF)
		{
			*(BYTE*)(siPtr) = vInt;
			vInt = (BYTE)GetNextInt(split);
			siPtr++;
		}
		//*(CHAR*)(sIntString+lstrlenA(sIntString)-1)=0;
		//free(sIntString);
		return sIntString;
	}

	VOID FreeString(CHAR*pStr) 
	{

		GlobalFree(pStr);
	}
	WCHAR *PCharToWideChar(CHAR* in_text)
	{

		if(!in_text)return 0;
		if(!*in_text)return 0;
		WCHAR *pret;
		DWORD dwNum;
		dwNum = MultiByteToWideChar(CP_ACP,NULL,in_text,-1,NULL,0);
		pret = (WCHAR*)GlobalAlloc(GPTR,dwNum*2+2);
		MultiByteToWideChar (CP_ACP,NULL,in_text,-1,pret,dwNum);
		return pret;
	}
	CHAR *WideCharToPChar(WCHAR* in_text)
	{

		if(!in_text)return 0;
		if(!*in_text)return 0;
		CHAR *pret;
		DWORD dwNum;
		dwNum = WideCharToMultiByte(CP_OEMCP,NULL,in_text,-1,NULL,0,NULL,FALSE);
		pret = (CHAR*)GlobalAlloc(GPTR,dwNum+1);
		WideCharToMultiByte (CP_OEMCP,NULL,in_text,-1,pret,dwNum,NULL,FALSE);
		return pret;
	}
	CHAR *WideCharToUtf8(WCHAR* in_text)
	{
		int len = WideCharToMultiByte(CP_UTF8, 0, in_text, -1, NULL, 0, NULL, NULL); 
		CHAR* szUtf8 = (CHAR*)GlobalAlloc(GPTR,len + 1); 
		ZeroMemory(szUtf8, len + 1); 
		WideCharToMultiByte (CP_UTF8, 0, in_text, -1, szUtf8, len, NULL,NULL); 
		return szUtf8; 
	}
	CHAR* PCharToUtf8(CHAR* in) 
	{ 
		CHAR *szUtf8;
		WCHAR * wszUtf8;
		int len;
		len=MultiByteToWideChar(CP_ACP, 0, in, -1, NULL,0); 
		wszUtf8 = new WCHAR[len+1];
		ZeroMemory(wszUtf8,len * 2 + 2); 
		MultiByteToWideChar(CP_ACP, 0, in, -1, wszUtf8, len); 

		len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL); 
		szUtf8=(CHAR*)GlobalAlloc(GPTR,len + 1); 
		ZeroMemory(szUtf8, len + 1); 
		WideCharToMultiByte (CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL,NULL); 
		delete[] wszUtf8;
		return szUtf8; 
	}
	BOOL HexStr2Hex(WCHAR* InStr, BYTE* OutStr)
	{
		if(!OutStr)return FALSE;
		CHAR *p = WideCharToPChar(InStr);
		HexStr2Hex(p,OutStr);
		FreeString(p);
		return TRUE;
	}
	CHAR *PUTF8ToChar(CHAR *szInfo)
	{

		int nCount = MultiByteToWideChar(CP_UTF8, 0, szInfo, -1, NULL, 0);
		WCHAR *pWideChar = new WCHAR [nCount + 1];
		ZeroMemory(pWideChar, (nCount + 1) * sizeof(WCHAR));
		MultiByteToWideChar(CP_UTF8, 0, szInfo, -1, pWideChar, nCount);

		//return szwchar;


		CHAR *pret;
		DWORD dwNum;
		dwNum = WideCharToMultiByte(CP_OEMCP,NULL,pWideChar,-1,NULL,0,NULL,FALSE);
		pret = (CHAR*)GlobalAlloc(GPTR,dwNum+1);
		WideCharToMultiByte (CP_OEMCP,NULL,pWideChar,-1,pret,dwNum,NULL,FALSE);
		delete []pWideChar;

		return pret;

	}

	BOOL HexStr2Hex(CHAR* InStr, BYTE* OutStr) 
	{ 
		if(!OutStr)return FALSE;
		DWORD i = 0, j = 0; 
		DWORD len = 0; 
		BYTE* Str=OutStr;

		len = lstrlenA(InStr)-1;
		Str = (BYTE*)GlobalAlloc(GPTR,len+2);
		if ((len%2)!=0) 
		{ 
			GlobalFree(Str);

			return FALSE; 
		} 

		ZeroMemory(OutStr, len/2); 

		lstrcpyA((char*)(Str), InStr); 


		for(i = 0; i < len; i++) 
		{ 
			if ((Str[i] > 47) && (Str[i] < 58)) 
			{ 
				Str[i] -= 48; 
			} 
			else if ((Str[i] > 64) && (Str[i] < 71)) 
			{ 
				Str[i] -= 55; 
			} 
			else if ((Str[i] > 96) && (Str[i] < 103)) 
			{ 
				Str[i] -= 87; 
			} 
			else 
			{ 
				GlobalFree(Str);
				return FALSE; 
			} 
		} 

		i = 0; 
		while(i < len) 
		{ // ×ª»» 
			OutStr[j] = Str[i] * 16 + Str[i + 1]; 
			i += 2; 
			j++; 
		} 

		GlobalFree(Str);
		return TRUE;
	}
  
}