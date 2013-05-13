#pragma once
#include <vector>
#include <wincrypt.h>	//crypt signal
using namespace std;

//////////////////////////////////////////////////////////////////////////
// PE 数字签名
#define ENCODING (X509_ASN_ENCODING | PKCS_7_ASN_ENCODING)
typedef struct {
	LPWSTR lpszProgramName;
	LPWSTR lpszPublisherLink;
	LPWSTR lpszMoreInfoLink;
} SPROG_PUBLISHERINFO, *PSPROG_PUBLISHERINFO;

typedef struct {
	WCHAR wszSerialNum[MAX_PATH];
	WCHAR wszIssuerName[MAX_PATH];
	WCHAR wszSubjectName[MAX_PATH];
} SSIGNER_INFO, *PSSIGNER_INFO;


class CSignerChecker
{
public:
	LPWSTR AllocateAndCopyWideString(LPCWSTR inputString);
	BOOL GetProgAndPublisherInfo(PCMSG_SIGNER_INFO pSignerInfo,	PSPROG_PUBLISHERINFO Info);
	BOOL GetDateOfTimeStamp(PCMSG_SIGNER_INFO pSignerInfo, SYSTEMTIME *st);
	BOOL GetTimeStampSignerInfo(PCMSG_SIGNER_INFO pSignerInfo, PCMSG_SIGNER_INFO *pCounterSignerInfo);
	BOOL PrintCertificateInfo(PCCERT_CONTEXT pCertContext, PSSIGNER_INFO psinfo);
};

#define FULLPATH		CString
#define FILENAME		CString
enum SEARCHLEVEL
{
	NO_LIMIT = -1,
	CURRENT_DIR	// 继续向下加便为指定目录层数
};

class CFileHelper : public CSignerChecker
{
public:
	CFileHelper(void);
	virtual ~CFileHelper(void);
	BOOL FindFilesInDirecotry(CString fileName,CString directory, vector<pair<FILENAME,FULLPATH>>& vctOut );
	BOOL FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount, vector<pair<FILENAME,FULLPATH>>& vctOut);
	vector<pair<FILENAME,FULLPATH>> FindFilesInDirecotry(CString fileName,CString directory );
	vector<pair<FILENAME,FULLPATH>> FindFilesInDirecotryRecursion( CString fileName,CString directory,int recursionCount);
	BOOL GetAppProductVersion(WCHAR* pszPath, VS_FIXEDFILEINFO* pVS);
	int GetPhysicalMAC(char*  mac);	//获取物理MAC Buffer长度 12+1
	bool IsPe(CString strFileName);
	BOOL GetPESections(CString strFileName, vector<CString>* pvcSectionNames,bool bAccessReadOnly = false);
	BOOL GetPESections(BYTE* lpbFileMap, vector<CString>* pvcSectionNames, bool bAccessReadOnly = false);
	bool GetCpuID(char *pszCpuID);
	
	PBYTE GetFileMapping(CString strFileName, bool bAccessReadOnly = false);
	BOOL ReleaseFileMapping(void* lpFMBase);
	//////////////////////////////////////////////////////////////////////////
	// PE 数字签名
	BOOL GetSignerInfoSimply(CString strFileName, PSSIGNER_INFO psinfo);
	
	//////////////////////////////////////////////////////////////////////////
	
};