#include "stdafx.h"
#include "vista.h"

BOOL Vista::IsVista()
{
	return (DWORD)(LOBYTE(LOWORD(GetVersion()))) > 5;
}

DWORD Vista::GetIntegrityLevel()
{
	DWORD result = IL_NOTAPPLICABLE;

	if (!IsVista()) 
		return result;

	HANDLE hToken;
	HANDLE hProcess;

	DWORD dwLengthNeeded;
	DWORD dwError = ERROR_SUCCESS;

	PTOKEN_MANDATORY_LABEL pTIL = NULL;
	DWORD dwIntegrityLevel;

	hProcess = GetCurrentProcess();
	if (OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken)) 
	{
		if (!GetTokenInformation(hToken, (::TOKEN_INFORMATION_CLASS)TokenIntegrityLevel, NULL, 0, &dwLengthNeeded))
		{
			dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER)
			{
				pTIL = (PTOKEN_MANDATORY_LABEL)LocalAlloc(0, dwLengthNeeded);
				if (pTIL != NULL)
				{
					if (GetTokenInformation(hToken, (::TOKEN_INFORMATION_CLASS)TokenIntegrityLevel, pTIL, dwLengthNeeded, &dwLengthNeeded))
					{
						dwIntegrityLevel = *GetSidSubAuthority(pTIL->Label.Sid, 
							(DWORD)(UCHAR)(*GetSidSubAuthorityCount(pTIL->Label.Sid)-1));

						if (dwIntegrityLevel < SECURITY_MANDATORY_MEDIUM_RID)
						{
							result = IL_LOW;								
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_MEDIUM_RID && 
							dwIntegrityLevel < SECURITY_MANDATORY_HIGH_RID)
						{
							result = IL_MEDIUM;
						}
						else if (dwIntegrityLevel >= SECURITY_MANDATORY_HIGH_RID)
						{
							result = IL_HIGH;
						}
					}

					LocalFree(pTIL);
				}
			}
		}
		CloseHandle(hToken);
	}

	return result;
}

PSID Vista::GetCurrentUserSid(PTOKEN_USER& usr)
{
	PSID pRet = NULL;
	HANDLE hToken = NULL, hProcess = GetCurrentProcess();

	DWORD dwSize = 0;

	if(OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		GetTokenInformation(hToken, (::TOKEN_INFORMATION_CLASS)TokenUser, usr, 0, &dwSize);
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			usr = (PTOKEN_USER)HeapAlloc(GetProcessHeap(), 0, dwSize+4);
			if(GetTokenInformation(hToken, (::TOKEN_INFORMATION_CLASS)TokenUser, usr, dwSize+4, &dwSize))
				pRet = usr->User.Sid;			
		}
		CloseHandle(hToken);
	}
	return pRet;
}

CString Vista::GetCurrentUserSidString()
{
	PTOKEN_USER usr = NULL;
	int n = 0;
	PSID pSid = NULL;
	CString strRet, tmp;
	PSID_IDENTIFIER_AUTHORITY sia; 
	PUCHAR pCnt = NULL;
	PDWORD pSub = NULL;
	pSid = GetCurrentUserSid(usr);
	if(pSid && IsValidSid(pSid))
	{
		sia = GetSidIdentifierAuthority(pSid);
		if (sia->Value[0] != 0 || sia->Value[1] != 0)
			strRet.Format(_T("%s0x%.2x%.2x%.2x%.2x%.2x%.2x-"), _T("S-1-"), 
			sia->Value[0], sia->Value[1], sia->Value[2], sia->Value[3],
			sia->Value[4], sia->Value[5]);
		else
		{
			n = (sia->Value[2] << 8);
			n = ((n + sia->Value[3]) << 8);
			n = ((n + sia->Value[4]) << 8);
			n = (n + sia->Value[5]);
			strRet.Format(_T("%s%d"), _T("S-1-"), n);
		}
		pCnt = GetSidSubAuthorityCount(pSid);
		for(n=0; n<(*pCnt); n++)
		{
			pSub = GetSidSubAuthority(pSid, n);
			if(GetLastError())
				break;
			tmp.Format(_T("-%u"), (*pSub));
			strRet += tmp;
		}
	}
	HeapFree(GetProcessHeap(), 0, usr);	
	return strRet;
}

