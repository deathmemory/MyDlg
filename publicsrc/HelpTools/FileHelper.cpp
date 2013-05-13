#pragma once
#include "StdAfx.h"
#include "FileHelper.h"

#include <WINNT.H>	//PE
#pragma comment(lib, "Version.lib")

#include <wintrust.h>
#pragma comment(lib, "crypt32.lib")

CFileHelper::CFileHelper(void)
{
}


CFileHelper::~CFileHelper(void)
{
}


/************************************************************************
说明：
    在文件夹中查找文件（仅在指定文件夹查找，不递归）
参数：
    fileName:要查找的文件名。支持通配符*
    directory:要查找的文件所在的目录路径
返回值：
    pair<string,string>类型的数组，map<文件名，文件路径>
************************************************************************/
vector<pair<FILENAME,FULLPATH>> CFileHelper::FindFilesInDirecotry(CString fileName,CString directory )
{
    vector<pair<CString,CString>> files;
 
    if(directory.Right(1) != "\\") //保证目录是以\结尾的 
    {   
        directory += "\\"; 
    }
    directory += fileName;
    CFileFind finder;
    BOOL success = finder.FindFile(directory);
    while(success)
    {
        success = finder.FindNextFile();
        CString name = finder.GetFileName();
        CString path = finder.GetFilePath();
        pair<CString,CString> fileMap(name,path);
        files.push_back(fileMap);
    }
    finder.Close();
    return files;
}
 
 
/************************************************************************
说明：
    在文件夹中递归查找文件
参数：
    fileName:要查找的文件名。支持通配符*
    directory:要查找的文件所在的目录路径
    recursionCount:递归查找的深度，默认为-1（不限制深度）,0表示只搜索给定的directory目录
返回值：
    pair<string,string>类型的数组，map<文件名，文件路径>
************************************************************************/
vector<pair<FILENAME,FULLPATH>> CFileHelper::FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount)
{
    vector<pair<CString,CString>> files;
 
    int curRecursionCount = recursionCount;
 
    //先对参数传进来的文件夹查找
    vector<pair<CString,CString>> localFiles =  FindFilesInDirecotry(fileName,directory);
    files.insert(files.end(),localFiles.begin(),localFiles.end());
 
    //然后对此文件夹下面的子文件夹递归查找
    CFileFind finder; 
    if(directory.Right(1) != _T("\\")) //保证目录是以\结尾的
    {   
        directory += _T("\\"); 
    }
    directory += _T("*.*"); 
 
    BOOL res = finder.FindFile(directory); 
    while (res)
    {
        res = finder.FindNextFile();
        CString path = finder.GetFilePath();
        CString name = finder.GetFileName();
        curRecursionCount = recursionCount;  //查找同级目录的时候将当前深度设置为给定值
        if(finder.IsDirectory() && !finder.IsDots()) 
        {
            if(curRecursionCount >= 1 || curRecursionCount == -1 )
            {
                if(curRecursionCount >= 1)
                {
                    curRecursionCount--;
                }
                localFiles = FindFilesInDirecotryRecursion(fileName,path,curRecursionCount); //如果目标是个文件夹，则利用嵌套来遍历 
                files.insert(files.end(),localFiles.begin(),localFiles.end());
            }
        }
    }
    finder.Close();
    return files;
}
/************************************************************************
说明：
    在文件夹中查找文件（仅在指定文件夹查找，不递归）
参数：
    fileName:要查找的文件名。支持通配符*
    directory:要查找的文件所在的目录路径
返回值：
    pair<string,string>类型的数组，map<文件名，文件路径>
************************************************************************/
BOOL CFileHelper::FindFilesInDirecotry(CString fileName,CString directory, vector<pair<FILENAME,FULLPATH>>& vctOut )
{
    if(directory.Right(1) != "\\") //保证目录是以\结尾的 
    {   
        directory += "\\"; 
    }
    directory += fileName;
    CFileFind finder;
    BOOL success = finder.FindFile(directory);
    while(success)
    {
        success = finder.FindNextFile();
        CString name = finder.GetFileName();
        CString path = finder.GetFilePath();
        pair<CString,CString> fileMap(name,path);
        vctOut.push_back(fileMap);
    }
    finder.Close();
    return success;
}
 
 
/************************************************************************
说明：
    在文件夹中递归查找文件
参数：
    fileName:要查找的文件名。支持通配符*
    directory:要查找的文件所在的目录路径
    recursionCount:递归查找的深度，默认为-1（不限制深度）,0表示只搜索给定的directory目录
返回值：
    pair<string,string>类型的数组，map<文件名，文件路径>
************************************************************************/
BOOL CFileHelper::FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount, vector<pair<FILENAME,FULLPATH>>& vctOut)
{
	BOOL bRet = FALSE;
    int curRecursionCount = recursionCount;
 
    //先对参数传进来的文件夹查找
	bRet = FindFilesInDirecotry(fileName,directory, vctOut);
 
    //然后对此文件夹下面的子文件夹递归查找
    CFileFind finder; 
    if(directory.Right(1) != _T("\\")) //保证目录是以\结尾的
    {   
        directory += _T("\\"); 
    }
    directory += _T("*.*"); 
 
    BOOL res = finder.FindFile(directory); 
    while (res)
    {
        res = finder.FindNextFile();
        CString path = finder.GetFilePath();
        CString name = finder.GetFileName();
        curRecursionCount = recursionCount;  //查找同级目录的时候将当前深度设置为给定值
        if(finder.IsDirectory() && !finder.IsDots()) 
        {
            if(curRecursionCount >= 1 || curRecursionCount == -1 )
            {
                if(curRecursionCount >= 1)
                {
                    curRecursionCount--;
                }
                bRet = FindFilesInDirecotryRecursion(fileName,path,curRecursionCount, vctOut); //如果目标是个文件夹，则利用嵌套来遍历 
            }
        }
    }
    finder.Close();
    return bRet;
}
BOOL CFileHelper::GetAppProductVersion(WCHAR* pszPath, VS_FIXEDFILEINFO* pVS)
{
	VS_FIXEDFILEINFO verInfo;
	DWORD dwHandle = 0;
	DWORD dwSize = GetFileVersionInfoSize(pszPath, &dwHandle);
	if (dwSize != 0)
	{              
		BYTE* pData = new BYTE[dwSize];
		if ( GetFileVersionInfo(pszPath, dwHandle, dwSize, pData) ) 
		{
			UINT pBufLen = 0;              
			VS_FIXEDFILEINFO* pVS_ = NULL;
			//if ( VerQueryValue(pData, _T("/StringFileInfo/080404e4/ProductVersion"), &pBuf, &pBufLen))
			if ( VerQueryValue(pData, _T("\\"), (LPVOID*)&pVS_, &pBufLen))
			{
				if (pVS_ && pVS)
				{
					memcpy(pVS, pVS_, sizeof(VS_FIXEDFILEINFO));
					delete [] pData;      
					return true;
				}
			}
		}
		delete [] pData;      
	}
	return false;
}

/************************************************************************/
/* 主要功能: 获取本机物理网卡地址									        */
/* 输入: 接收返回的物理网卡(MAC)地址										*/
/* 作者: 解云飞		日期: 2011/01/14										*/
/************************************************************************/
#include <Iphlpapi.h>
#pragma comment (lib, "Iphlpapi.lib")
int CFileHelper::GetPhysicalMAC(char* pszMAC )
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *) malloc( sizeof(IP_ADAPTER_INFO) );
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	/* 做一个初始调用GetAdaptersInfo进入了ulOutBufLen变量所需的大小	*/
	if (GetAdaptersInfo( pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS )		// 第一步是获取所有网卡的个数的总长度,返回给 ulOutBufLen
	{
		pAdapterInfo = (IP_ADAPTER_INFO *) realloc((void*)pAdapterInfo , ulOutBufLen);	//重新分配总长度大小的内存空间
	}

	if ((dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{
		pAdapter = pAdapterInfo;
		while (pAdapter) 
		{
			int iDesLen = strlen(pAdapter->Description);
			for (int i = 0; i < iDesLen; ++ i )
			{
				pAdapter->Description[i] = toupper(pAdapter->Description[i]);	/*将关键判断的描述全部转换成大写字母, 以保证其可靠性
																				[返回的关键描述字符一般都为大写字母, 这一步可以省略, 但为了可靠性,还是加上比较好]*/
			}
			//if (strstr(pAdapter->Description, "PCI"))
			if (pAdapter)
			{
				for (UINT i = 0; i < pAdapter->AddressLength; i++)
					sprintf(pszMAC + i * 2, "%02X", pAdapter->Address[i]);	//i * 2 中的2 与 %02X 中的2 相关, 以保证一致性及正确性
				return 0;
			}
			pAdapter = pAdapter->Next;
		}
	}
	else
		return -1;

	return 0;
}

/************************************************************************/
/* 主要功能: 判断是否为PE应用程序
/* 实现思路: 检查PE头是否为MZ, 根据PE文件结构定位到PE特征处
/************************************************************************/
bool CFileHelper::IsPe(CString strFileName)
{
	BOOL bRet = FALSE;
	CFile tfile;
	if (!tfile.Open(strFileName, CFile::modeRead))
		return false;
	int nDosHeader = sizeof(IMAGE_DOS_HEADER);
	char* pBuff = new char [nDosHeader + 1];
	ZeroMemory(pBuff, nDosHeader +1);
	tfile.Read(pBuff, nDosHeader);
	if (memcmp(pBuff, "MZ", 2) != 0)
	{
		goto flagReturn;
	}
	else
	{
		PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) pBuff;
		DWORD dwPeAddr = pDosHeader->e_lfanew;
		tfile.Seek(dwPeAddr, 0);
		tfile.Read(pBuff, 2);
		pBuff[2] = 0;
		if (memcmp(pBuff, "PE", 2) != 0)
			goto flagReturn;
		else
			bRet = true;
	}

flagReturn:
	if (pBuff)
		delete [] pBuff;
	return bRet;
}

BOOL CFileHelper::GetPESections(CString strFileName, vector<CString>* pvcSectionNames,bool bAccessReadOnly/* = false*/)
{
	BOOL bRet = FALSE;
	PBYTE lpbFileMap = this->GetFileMapping(strFileName, bAccessReadOnly);
	if (lpbFileMap)
	{
		bRet = GetPESections(lpbFileMap, pvcSectionNames, bAccessReadOnly);
		UnmapViewOfFile(lpbFileMap);
	}
	return bRet;
}

BOOL CFileHelper::GetPESections(BYTE* lpbFileMap, vector<CString>* pvcSectionNames,bool bAccessReadOnly/* = false*/)
{
	if ( IsBadReadPtr(lpbFileMap, 4) ) return FALSE;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER) lpbFileMap;
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS) (lpbFileMap + pDosHeader->e_lfanew);
	int nSectionCount = pNtHeaders->FileHeader.NumberOfSections;
	if ( nSectionCount <=0 )	 return FALSE;	
	int nOffset = sizeof(IMAGE_NT_HEADERS);
	PIMAGE_SECTION_HEADER pSectionHeaders = PIMAGE_SECTION_HEADER(lpbFileMap + pDosHeader->e_lfanew + nOffset);
	for ( int i = 0; i < nSectionCount; ++ i )
	{
		pvcSectionNames->push_back(CString(pSectionHeaders[i].Name));
	}

	return TRUE;
}

/************************************************************************/
/* 主要功能: 获取CPU 序列号									            */
/* 输入: 一个char数组用来存储获取的CPU 序列号								*/
/* 输出: CPU支持查询返回True , 否则返回 False								*/
/* 作者: 解云飞		日期: 2010/11/15										*/
/************************************************************************/

bool CFileHelper::GetCpuID(char *pszCpuID)
{
	unsigned int iEAX,iEBX,iECX,iEDX;
	iEAX = iEBX = iECX = iEDX = 0;
	_asm pushad;
	_asm
	{
		mov eax, 01h;
		xor ecx, ecx;
		xor edx, edx;
		cpuid;
		mov iEDX, edx;
		mov iEAX, eax;
	}
	if (!iEDX & (1 << 18))		//检测是否支持查询CPU
		return false;
	_asm
	{
		mov eax, 03h;
		cpuid;
		mov iECX, ecx;
		mov iEDX, edx;
	}
	_asm popad;
	sprintf(pszCpuID,"%08x%08x%08x",iEAX,iECX,iEDX) ;

	return true;

}

BOOL CFileHelper::GetSignerInfoSimply(CString strFileName, PSSIGNER_INFO psinfo)
{
	DWORD dwEncoding, dwContentType, dwFormatType;
	HCERTSTORE hStore = NULL;
	HCRYPTMSG hMsg = NULL;
	BOOL fResult = CryptQueryObject(CERT_QUERY_OBJECT_FILE,
		strFileName,
		CERT_QUERY_CONTENT_FLAG_PKCS7_SIGNED_EMBED,
		CERT_QUERY_FORMAT_FLAG_BINARY,
		0,
		&dwEncoding,
		&dwContentType,
		&dwFormatType,
		&hStore,
		&hMsg,
		NULL);
	if (fResult)
	{
		DWORD dwSignerInfo = 0;
		fResult = CryptMsgGetParam(hMsg,
			CMSG_SIGNER_INFO_PARAM,
			0,
			NULL,
			&dwSignerInfo);
		if (fResult)
		{
			PCMSG_SIGNER_INFO pSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSignerInfo);
			fResult = CryptMsgGetParam(hMsg,
				CMSG_SIGNER_INFO_PARAM,
				0,
				(PVOID)pSignerInfo,
				&dwSignerInfo);
			if (fResult)
			{
				SPROG_PUBLISHERINFO ProgPubInfo;
				ZeroMemory(&ProgPubInfo, sizeof(SPROG_PUBLISHERINFO));
				// 				if (GetProgAndPublisherInfo(pSignerInfo, &ProgPubInfo))
				// 				{
				// 					if (ProgPubInfo.lpszProgramName != NULL)
				// 					{
				// 						wprintf(L"Program Name : %s\n",
				// 							ProgPubInfo.lpszProgramName);
				// 					}
				// 					if (ProgPubInfo.lpszPublisherLink != NULL)
				// 					{
				// 						wprintf(L"Publisher Link : %s\n",
				// 							ProgPubInfo.lpszPublisherLink);
				// 					}
				// 					if (ProgPubInfo.lpszMoreInfoLink != NULL)
				// 					{
				// 						wprintf(L"MoreInfo Link : %s\n",
				// 							ProgPubInfo.lpszMoreInfoLink);
				// 					}
				// 				}
				CERT_INFO CertInfo;
				ZeroMemory(&CertInfo, sizeof(CERT_INFO));
				// Search for the signer certificate in the temporary
				// certificate store.
				CertInfo.Issuer = pSignerInfo->Issuer;
				CertInfo.SerialNumber = pSignerInfo->SerialNumber;
				PCCERT_CONTEXT pCertContext = CertFindCertificateInStore(hStore,
					ENCODING,
					0,
					CERT_FIND_SUBJECT_CERT,
					(PVOID)&CertInfo,
					NULL);
				if (pCertContext)
				{
					CSignerChecker::PrintCertificateInfo(pCertContext, psinfo);
				}
			}
			LocalFree(pSignerInfo);
		}
	}

	return fResult;
}

PBYTE CFileHelper::GetFileMapping(CString strFileName, bool bAccessReadOnly/* = false*/)
{
	PBYTE ptchRet = NULL;
	DWORD dwAccessValue = 0;
	if (bAccessReadOnly)	dwAccessValue = GENERIC_READ;
	else dwAccessValue = GENERIC_WRITE | GENERIC_READ;
	//HANDLE hFile = CreateFile(strFileName, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hFile = CreateFile(strFileName, dwAccessValue, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		if (bAccessReadOnly)	dwAccessValue = PAGE_READONLY;
		else dwAccessValue = PAGE_READWRITE;
		HANDLE hFileMapping = CreateFileMapping(hFile,NULL,PAGE_READONLY, 0, 0, NULL);
		if (INVALID_HANDLE_VALUE != hFileMapping)
		{
			// 得到系统分配粒度
			SYSTEM_INFO SysInfo;
			GetSystemInfo(&SysInfo);
			DWORD dwGran = SysInfo.dwAllocationGranularity;
			// 得到文件尺寸
			DWORD dwFileSizeHigh;
			__int64 qwFileSize = GetFileSize(hFile, &dwFileSizeHigh);
			qwFileSize |= (((__int64)dwFileSizeHigh) << 32);
			// 偏移地址 
			__int64 qwFileOffset = 0;
			// 块大小
			DWORD dwBlockBytes = 1000 * dwGran;
			if (qwFileSize < 1000 * dwGran)
				dwBlockBytes = (DWORD)qwFileSize;
			if (qwFileOffset >= 0)
			{
				// 映射视图
				if (bAccessReadOnly)	dwAccessValue = FILE_MAP_READ;
				else dwAccessValue = FILE_MAP_ALL_ACCESS;
				PBYTE lpbMapAddress = (PBYTE)MapViewOfFile(hFileMapping,FILE_MAP_READ, 0, 0, dwBlockBytes);
				if (lpbMapAddress)
				{
					ptchRet = lpbMapAddress;
				}
			}
			CloseHandle(hFileMapping);
		}
		CloseHandle(hFile);
	}

	return ptchRet;
}

BOOL CFileHelper::ReleaseFileMapping(void* lpFMBase)
{
	return UnmapViewOfFile(lpFMBase);
}
//////////////////////////////////////////////////////////////////////////
// CSignerChecker

BOOL CSignerChecker::GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,
	PSPROG_PUBLISHERINFO Info)
{
	BOOL fReturn = FALSE;
	PSPC_SP_OPUS_INFO OpusInfo = NULL;
	DWORD dwData;
	BOOL fResult;
	__try
	{
		// Loop through authenticated attributes and find
		// SPC_SP_OPUS_INFO_OBJID OID.
		for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
		{
			if (lstrcmpA(SPC_SP_OPUS_INFO_OBJID,
				pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
			{
				// Get Size of SPC_SP_OPUS_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					SPC_SP_OPUS_INFO_OBJID,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwData);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}
				// Allocate memory for SPC_SP_OPUS_INFO structure.
				OpusInfo = (PSPC_SP_OPUS_INFO)LocalAlloc(LPTR, dwData);
				if (!OpusInfo)
				{
					_tprintf(_T("Unable to allocate memory for Publisher Info.\n"));
					__leave;
				}
				// Decode and get SPC_SP_OPUS_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					SPC_SP_OPUS_INFO_OBJID,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					OpusInfo,
					&dwData);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}
				// Fill in Program Name if present.
				if (OpusInfo->pwszProgramName)
				{
					Info->lpszProgramName =
						AllocateAndCopyWideString(OpusInfo->pwszProgramName);
				}
				else
					Info->lpszProgramName = NULL;
				// Fill in Publisher Information if present.
				if (OpusInfo->pPublisherInfo)
				{
					switch (OpusInfo->pPublisherInfo->dwLinkChoice)
					{
					case SPC_URL_LINK_CHOICE:
						Info->lpszPublisherLink =
							AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszUrl);
						break;
					case SPC_FILE_LINK_CHOICE:
						Info->lpszPublisherLink =
							AllocateAndCopyWideString(OpusInfo->pPublisherInfo->pwszFile);
						break;
					default:
						Info->lpszPublisherLink = NULL;
						break;
					}
				}
				else
				{
					Info->lpszPublisherLink = NULL;
				}
				// Fill in More Info if present.
				if (OpusInfo->pMoreInfo)
				{
					switch (OpusInfo->pMoreInfo->dwLinkChoice)
					{
					case SPC_URL_LINK_CHOICE:
						Info->lpszMoreInfoLink =
							AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszUrl);
						break;
					case SPC_FILE_LINK_CHOICE:
						Info->lpszMoreInfoLink =
							AllocateAndCopyWideString(OpusInfo->pMoreInfo->pwszFile);
						break;
					default:
						Info->lpszMoreInfoLink = NULL;
						break;
					}
				}
				else
				{
					Info->lpszMoreInfoLink = NULL;
				}
				fReturn = TRUE;
				break; // Break from for loop.
			} // lstrcmp SPC_SP_OPUS_INFO_OBJID
		} // for
	}
	__finally
	{
		if (OpusInfo != NULL) LocalFree(OpusInfo);
	}
	return fReturn;
}

BOOL CSignerChecker::GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st)
{
	BOOL fResult;
	FILETIME lft, ft;
	DWORD dwData;
	BOOL fReturn = FALSE;
	// Loop through authenticated attributes and find
	// szOID_RSA_signingTime OID.
	for (DWORD n = 0; n < pSignerInfo->AuthAttrs.cAttr; n++)
	{
		if (lstrcmpA(szOID_RSA_signingTime,
			pSignerInfo->AuthAttrs.rgAttr[n].pszObjId) == 0)
		{
			// Decode and get FILETIME structure.
			dwData = sizeof(ft);
			fResult = CryptDecodeObject(ENCODING,
				szOID_RSA_signingTime,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].pbData,
				pSignerInfo->AuthAttrs.rgAttr[n].rgValue[0].cbData,
				0,
				(PVOID)&ft,
				&dwData);
			if (!fResult)
			{
				_tprintf(_T("CryptDecodeObject failed with %x\n"),
					GetLastError());
				break;
			}
			// Convert to local time.
			FileTimeToLocalFileTime(&ft, &lft);
			FileTimeToSystemTime(&lft, st);
			fReturn = TRUE;
			break; // Break from for loop.
		} //lstrcmp szOID_RSA_signingTime
	} // for
	return fReturn;
}

BOOL CSignerChecker::GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO *pCounterSignerInfo)
{
	PCCERT_CONTEXT pCertContext = NULL;
	BOOL fReturn = FALSE;
	BOOL fResult;
	DWORD dwSize;
	__try
	{
		*pCounterSignerInfo = NULL;
		// Loop through unathenticated attributes for
		// szOID_RSA_counterSign OID.
		for (DWORD n = 0; n < pSignerInfo->UnauthAttrs.cAttr; n++)
		{
			if (lstrcmpA(pSignerInfo->UnauthAttrs.rgAttr[n].pszObjId,
				szOID_RSA_counterSign) == 0)
			{
				// Get size of CMSG_SIGNER_INFO structure.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					NULL,
					&dwSize);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}
				// Allocate memory for CMSG_SIGNER_INFO.
				*pCounterSignerInfo = (PCMSG_SIGNER_INFO)LocalAlloc(LPTR, dwSize);
				if (!*pCounterSignerInfo)
				{
					_tprintf(_T("Unable to allocate memory for timestamp info.\n"));
					__leave;
				}
				// Decode and get CMSG_SIGNER_INFO structure
				// for timestamp certificate.
				fResult = CryptDecodeObject(ENCODING,
					PKCS7_SIGNER_INFO,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].pbData,
					pSignerInfo->UnauthAttrs.rgAttr[n].rgValue[0].cbData,
					0,
					(PVOID)*pCounterSignerInfo,
					&dwSize);
				if (!fResult)
				{
					_tprintf(_T("CryptDecodeObject failed with %x\n"),
						GetLastError());
					__leave;
				}
				fReturn = TRUE;
				break; // Break from for loop.
			}
		}
	}
	__finally
	{
		// Clean up.
		if (pCertContext != NULL) CertFreeCertificateContext(pCertContext);
	}
	return fReturn;
}

LPWSTR CSignerChecker::AllocateAndCopyWideString(LPCWSTR inputString)
{
	LPWSTR outputString = NULL;
	outputString = (LPWSTR)LocalAlloc(LPTR,
		(wcslen(inputString) + 1) * sizeof(WCHAR));
	if (outputString != NULL)
	{
		lstrcpyW(outputString, inputString);
	}
	return outputString;
}

BOOL CSignerChecker::PrintCertificateInfo(PCCERT_CONTEXT pCertContext, PSSIGNER_INFO psinfo)
{
	BOOL fReturn = FALSE;
	LPTSTR szName = NULL;
	DWORD dwData;
	__try
	{
		// Print Serial Number.
		_tprintf(_T("Serial Number: "));
		dwData = pCertContext->pCertInfo->SerialNumber.cbData;
		for (DWORD n = 0; n < dwData; n++)
		{
			swprintf_s(&(psinfo->wszSerialNum[3*n]), MAX_PATH, _T("%02x "), pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
// 			_tprintf(_T("%02x "),
// 				pCertContext->pCertInfo->SerialNumber.pbData[dwData - (n + 1)]);
		}
		_tprintf(_T("\n"));
		// Get Issuer name size.
		if (!(dwData = CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG,
			NULL,
			NULL,
			0)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}
		// Allocate memory for Issuer name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			_tprintf(_T("Unable to allocate memory for issuer name.\n"));
			__leave;
		}
		// Get Issuer name.
		if (!(CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			CERT_NAME_ISSUER_FLAG,
			NULL,
			szName,
			dwData)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}
		// print Issuer name.
		_tprintf(_T("Issuer Name: %s\n"), szName);
		memcpy(psinfo->wszIssuerName, szName, dwData * sizeof(TCHAR));	// 
		LocalFree(szName);
		szName = NULL;
		// Get Subject name size.
		if (!(dwData = CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			NULL,
			0)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}
		// Allocate memory for subject name.
		szName = (LPTSTR)LocalAlloc(LPTR, dwData * sizeof(TCHAR));
		if (!szName)
		{
			_tprintf(_T("Unable to allocate memory for subject name.\n"));
			__leave;
		}
		// Get subject name.
		if (!(CertGetNameString(pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			szName,
			dwData)))
		{
			_tprintf(_T("CertGetNameString failed.\n"));
			__leave;
		}
		// Print Subject Name.
		_tprintf(_T("Subject Name: %s\n"), szName);
		memcpy(psinfo->wszSubjectName, szName, dwData * sizeof(TCHAR));
		fReturn = TRUE;
	}
	__finally
	{
		if (szName != NULL) LocalFree(szName);
	}
	return fReturn;
}
