#include "stdafx.h"
#include "registry.h"
#include "utils.h"
#include <vista.h>
#include "Config.h"
#include <LIMITS.H>

CString Registry::_companyKey = _T("");
CString Registry::_sid = _T("");
CString Registry::_browserVersion = _T("");

__forceinline CString Registry::GetKey()
{
	CString sKey = L"Software\\";
	if (Vista::IsVista() && Vista::GetIntegrityLevel() > Vista::IL_LOW)
	{
		sKey += L"Microsoft\\Internet Explorer\\InternetRegistry\\REGISTRY\\USER\\";
		sKey += (_sid.IsEmpty() ? (_sid = Vista::GetCurrentUserSidString()) : _sid);
		sKey += L"\\Software\\";
	}
	sKey += Config::Instance()->name;

	return sKey;
}

CString Registry::GetValue(const CString& name, const CString& def, const CString& subkey)
{
	CString val(def);

	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	CRegKey key;
	LONG error = key.Create(HKEY_CURRENT_USER, path);

	if(ERROR_SUCCESS == error)
	{
		ULONG length = 0;
		CString tmp;
		error = key.QueryStringValue(name, NULL, &length);
		error = key.QueryStringValue(name, tmp.GetBuffer(length), &length);
		tmp.ReleaseBuffer();
		if(ERROR_SUCCESS == error)
		{
			val = tmp;
		}
		key.Close();
	}
	return val;
}

LONG Registry::GetValue(const CString& name, VARIANT& val, const CString& subkey)
{
	return ERROR_SUCCESS;
}

LONG Registry::GetValue(const CString& name, DWORD& val, const CString& subkey)
{
	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	CRegKey key;
	LONG error = key.Create(HKEY_CURRENT_USER, path);

	if(ERROR_SUCCESS == error)
	{
		error = key.QueryDWORDValue(name, val);
	}

	return error;
}

LONG Registry::SetValue(const CString& name, DWORD val, const CString& subkey)
{
	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	CRegKey key;
	LONG lRes = key.Create(HKEY_CURRENT_USER, path);

	if(ERROR_SUCCESS == lRes)
	{
		lRes = key.SetDWORDValue(name, val);
	}

	return lRes;
}

LONG Registry::SetValue(const CString& name, const VARIANT& val, const CString& subkey)
{
	return ERROR_SUCCESS;
}

LONG Registry::SetValue(const CString& name, const CString& val, const CString& subkey)
{
	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	CRegKey key; 
	LONG error = key.Create(HKEY_CURRENT_USER, path);

	if(ERROR_SUCCESS == error)
	{
		error = key.SetStringValue(name, val);
		key.Close();
	}

	return error;
}

LONG Registry::GetValue(const CString& name, BSTR* pValue, ULONG* pnBytes, const CString& subkey)
{
	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	CRegKey key;
	LONG error = key.Create(HKEY_CURRENT_USER, path);

	if(ERROR_SUCCESS == error)
	{
		error = key.QueryBinaryValue(name, NULL, pnBytes);
		*pValue = SysAllocStringByteLen('\0', *pnBytes);
		error = key.QueryBinaryValue(name, *pValue, pnBytes);
		key.Close();
	}
	return error;
}

LONG Registry::SetValue(const CString& name, const void* pData, ULONG nBytes, const CString& subkey)
{
	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	CRegKey key; 
	LONG error = key.Create(HKEY_CURRENT_USER, path);

	if(ERROR_SUCCESS == error)
	{
		error = key.SetBinaryValue(name, pData, nBytes);
		key.Close();
	}

	return error;
}

LONG Registry::RemoveValue(const CString& name, const CString& subkey)
{
	CRegKey hKey;
	CString strValue;

	CString path = Registry::GetKey();
	if (!subkey.IsEmpty())
		path = L"\\" + subkey;

	LONG error = hKey.Open(HKEY_CURRENT_USER, path);
	if (error != ERROR_SUCCESS)
		return error;

	error = hKey.DeleteValue(name);

	hKey.Close();

	return error;
}

LONG Registry::RemoveAll()
{
	CString sKey = L"Software\\";
	if (Vista::IsVista() && Vista::GetIntegrityLevel() > Vista::IL_LOW)
	{
		sKey += L"Microsoft\\Internet Explorer\\InternetRegistry\\REGISTRY\\USER\\";
		sKey += (_sid.IsEmpty() ? (_sid = Vista::GetCurrentUserSidString()) : _sid);
		sKey += L"\\Software\\";
	}

	CRegKey key; 
	LONG error = key.Create(HKEY_CURRENT_USER, sKey);

	if (ERROR_SUCCESS == error)
	{
		error = key.RecurseDeleteKey(Config::Instance()->name);
		key.Close();
	}

	return error;
}

CString Registry::GetInternetExplorerVersionFromRegistry()
{
	if (_browserVersion.GetLength() > 0)
		return _browserVersion;

	CString retVal;
	CRegKey k;
	if (k.Open(HKEY_LOCAL_MACHINE,_T("Software\\Microsoft\\Internet Explorer"), KEY_READ) == ERROR_SUCCESS)
	{
		TCHAR szBuf[1024] = { 0 };
		DWORD dwBufLen = 1024;			
		
		if (ERROR_SUCCESS != k.QueryStringValue(_T("svcVersion"), szBuf, &dwBufLen))
		{
			ZeroMemory(szBuf, sizeof(szBuf));
			dwBufLen = 1024;

			k.QueryStringValue(_T("Version"), szBuf, &dwBufLen);
		}
		
		if (dwBufLen != 1024)
		{
			retVal = CString(szBuf);
		}
	}

	return retVal;
}

BOOL Registry::CheckInternetExplorerVersion(const CString& v, const CString& r)
{
	return (r.Find(v) == 0 ? TRUE : FALSE);
}

Registry::InternetExplorerVersion Registry::GetInternetExplorerVersion()
{
	InternetExplorerVersion retVal = ievUnknown;

	CString version = GetInternetExplorerVersionFromRegistry();

	if (CheckInternetExplorerVersion(_T("6."), version))
	{
		retVal = iev6;
	}
	else if (CheckInternetExplorerVersion(_T("7."), version))
	{
		retVal = iev7;
	}
	else if (CheckInternetExplorerVersion(_T("8."), version))
	{
		retVal = iev8;
	}
	else if (CheckInternetExplorerVersion(_T("9."), version))
	{
		retVal = iev9;
	}
	else if (CheckInternetExplorerVersion(_T("10."), version))
	{
		retVal = iev10;
	}
	else if (CheckInternetExplorerVersion(_T("11."), version))
	{
		retVal = iev11;
	}

	return retVal;
}

