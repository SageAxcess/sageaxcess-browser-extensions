#pragma once

#define REG_MAX_VALUE_LENGTH 1024*16

#include <atlstr.h>
#define _WTL_NO_CSTRING

class Registry
{
private:
 	static CString GetInternetExplorerVersionFromRegistry();
	static BOOL CheckInternetExplorerVersion(const CString& v, const CString& r);
	static CString GetKey();

public:
	static CString GetValue(const CString& name, const CString& def, const CString& subkey = _T(""));
	static LONG GetValue(const CString& name, VARIANT& val, const CString& subkey = _T(""));
	static LONG GetValue(const CString& name, DWORD& val, const CString& subkey = _T(""));
	static LONG GetValue(const CString& name, BSTR* pValue, ULONG* pnBytes, const CString& subkey = _T(""));
 	static LONG SetValue(const CString& name, const CString& val, const CString& subkey = _T(""));
	static LONG SetValue(const CString& name, const VARIANT& val, const CString& subkey = _T(""));
	static LONG SetValue(const CString& name, DWORD val, const CString& subkey = _T(""));
	static LONG SetValue(const CString& name, const void* pData, ULONG nBytes, const CString& subkey = _T(""));
 	static LONG RemoveValue(const CString& name, const CString& subkey = _T(""));
	static LONG RemoveAll();

	enum InternetExplorerVersion{iev6, iev7, iev8, iev9, iev10, iev11, ievUnknown};

	static InternetExplorerVersion GetInternetExplorerVersion();

private:
	static CString _companyKey;
	static CString _sid;
	static CString _browserVersion;
};






