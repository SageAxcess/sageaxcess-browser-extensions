#include "StdAfx.h"
#include "Utils.h"
#include <string>
#include <oleauto.h>
#include <sstream>
#include <Registry.h>
#include <vector>

#define BUFFER_SIZE 512

PTSTR Normalize(BYTE* pBuffer)
{
	PTSTR ptText; // pointer to the text char* or wchar_t* depending on UNICODE setting
	PWSTR pwStr; // pointer to a wchar_t buffer
	size_t nLength; // a useful integer variable

	// obtain a wide character pointer to check BOMs
	pwStr = reinterpret_cast<PWSTR>(pBuffer);

	// check if the first word is a Unicode Byte Order Mark
	if (*pwStr == 0xFFFE || *pwStr == 0xFEFF)
	{
		// Yes, this is Unicode data
		if (*pwStr++ == 0xFFFE)
		{
			// BOM says this is Big Endian so we need
			// to swap bytes in each word of the text
			while (*pwStr)
			{
				// swap bytes in each word of the buffer
				WCHAR wcTemp = *pwStr >> 8;
				wcTemp |= *pwStr << 8;
				*pwStr = wcTemp;
				++pwStr;
			}
			// point back to the start of the text
			pwStr = reinterpret_cast<PWSTR>(pBuffer + 2);
		}
#if !defined(UNICODE)
		// This is a non-Unicode project so we need
		// to convert wide characters to multi-byte

		// get calculated buffer size
		nLength = WideCharToMultiByte(CP_UTF8, 0, pwStr, -1, NULL, 0, NULL, NULL);
		// obtain a new buffer for the converted characters
		ptText = new TCHAR[nLength];
		// convert to multi-byte characters
		nLength = WideCharToMultiByte(CP_UTF8, 0, pwStr, -1, ptText, nLength, NULL, NULL);
#else
		nLength = wcslen(pwStr) + 1; // if Unicode, then copy the input text
		ptText = new WCHAR[nLength]; // to a new output buffer
		nLength *= sizeof(WCHAR); // adjust to size in bytes
		memcpy_s(ptText, nLength, pwStr, nLength);
#endif
	}
	else
	{
		// The text data is UTF-8 or Ansi
#if defined(UNICODE)
		// This is a Unicode project so we need to convert
		// multi-byte or Ansi characters to Unicode.

		// get calculated buffer size
		nLength = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<PCSTR>(pBuffer), -1, NULL, 0);
		// obtain a new buffer for the converted characters
		ptText = new TCHAR[nLength];
		// convert to Unicode characters
		nLength = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<PCSTR>(pBuffer), -1, ptText, nLength);
#else
		// This is a non-Unicode project so we just need
		// to skip the UTF-8 BOM, if present
		if (memcmp(pBuffer, "\xEF\xBB\xBF", 3) == 0)
		{
			// UTF-8
			pBuffer += 3;
		}
		nLength = strlen(reinterpret_cast<PSTR>(pBuffer)) + 1; // if UTF-8/ANSI, then copy the input text
		ptText = new char[nLength]; // to a new output buffer
		memcpy_s(ptText, nLength, pBuffer, nLength);
#endif
	}

	// return pointer to the (possibly converted) text buffer.
	return ptText;
}

CString utils::GetProgPath()
{
	CString progPath;
	TCHAR szFileName[MAX_PATH];	
	::GetModuleFileName(_AtlBaseModule.GetModuleInstance(), szFileName, MAX_PATH);
	::PathRemoveFileSpec(szFileName); 
	progPath = szFileName;
	progPath += _T('\\');
	return progPath;		
}

CString utils::GetFileExtension(const CString& str)
{
	CString ext = _T("");
	int i = str.ReverseFind(TCHAR('.'));
	if(i != -1)
	{
		ext = str.Mid(i+1);
		ext.MakeLower();
	}
	return ext;
}
	
HRESULT utils::ExposeObject(IWebBrowser2* pBrowser, LPCWSTR exposeName, IDispatch* pExpose)
{
	HRESULT hr = S_OK;
	CComPtr<IDispatchEx> spScript;

	if (SUCCEEDED(hr))
	{
		CComPtr<IDispatch> spDocumentDisp;
		hr = pBrowser->get_Document(&spDocumentDisp);
		CComQIPtr<IHTMLDocument2> spDocument = spDocumentDisp;
		CComPtr<IDispatch> spDe1;
		if (SUCCEEDED(hr) && spDocument)
			hr = spDocument->get_Script(&spDe1);
		if (!spDe1)
			return S_FALSE;
		if (SUCCEEDED(hr))
			hr = spDe1->QueryInterface(IID_IDispatchEx, (void**)&spScript);
	}

	if (SUCCEEDED(hr))
	{
		// Add new members to an object ("expando")—use GetDispID with the fdexNameEnsure flag.
		DISPID dispId = 0;
		DISPPARAMS dispparams, dispparamsNoArgs = {NULL, NULL, 0, 0};
		VARIANT var;

		hr = spScript->GetDispID(CComBSTR(L"window"), 0, &dispId);
		if (SUCCEEDED(hr))
			hr = spScript->InvokeEx(dispId, LOCALE_USER_DEFAULT, 
			DISPATCH_PROPERTYGET, &dispparamsNoArgs, 
			&var, NULL, NULL);
		spScript = NULL;
		if (SUCCEEDED(hr))
			hr = var.pdispVal->QueryInterface(IID_IDispatchEx, (void**)&spScript);
		if (SUCCEEDED(hr))
			hr = spScript->GetDispID(CComBSTR(exposeName), fdexNameEnsure, &dispId);

		if (SUCCEEDED(hr))
		{
			DISPID putId = DISPID_PROPERTYPUT;
			var.vt = VT_DISPATCH;
			var.pdispVal = pExpose;
			dispparams.rgvarg = &var;
			dispparams.rgdispidNamedArgs = &putId;
			dispparams.cArgs = 1;
			dispparams.cNamedArgs = 1;
			hr = spScript->InvokeEx(dispId, LOCALE_USER_DEFAULT, 
				DISPATCH_PROPERTYPUT, &dispparams,
				NULL, NULL, NULL);
		}
	}

	return hr;
}

// IWebBrowser2 to IHTMLWindow2
CComQIPtr<IHTMLWindow2> utils::IWebBrowserToIHTMLWindow(CComQIPtr<IWebBrowser2> spBrowser)
{
	ATLASSERT(spBrowser != NULL);
	CComQIPtr<IHTMLWindow2> spWindow;

	// Get the document of the browser.
	CComQIPtr<IDispatch> spDisp;
	HRESULT hr = spBrowser->get_Document(&spDisp);

	if (SUCCEEDED(hr) && 
		spDisp)
	{
		// Get the window of the document.
		CComQIPtr<IHTMLDocument2> spDoc = spDisp;
		if (spDoc != NULL)
		{
			hr = spDoc->get_parentWindow(&spWindow);
		}
	}

	if (NULL == spWindow)
	{
		spWindow = spBrowser;
	}


	return spWindow;
}

CStringA utils::GetFileDataA(const CString& path)
{
	HANDLE hFile;
	DWORD dwBytesRead;

	CStringA res = "";

	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return res;
	}

	do
	{
		char buff[4096];
		if (ReadFile(hFile, buff, 4096, &dwBytesRead, NULL))
		{
			CStringA tmp(buff);
			tmp.Truncate(dwBytesRead);
			res += tmp;
		}
	}
	while (dwBytesRead == 4096);

	CloseHandle (hFile);

	return res;
}

CString utils::GetFileData(const CString& path)
{
	HANDLE hFile;
	DWORD dwBytesRead;

	CString res = _T("");
	CStringA resA = "";

	hFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return res;
	}

	do
	{
		char buff[4096];
		if (ReadFile(hFile, buff, 4096, &dwBytesRead, NULL))
		{
			CStringA tmp(buff);
			tmp.Truncate(dwBytesRead);
			resA += tmp;
		}
	}
	while (dwBytesRead == 4096);

	CloseHandle (hFile);

	PTSTR x = Normalize((PBYTE)resA.GetBuffer());

	res = x;
	delete [] x;

	return res;
}


HRESULT utils::VariantToString(const VARIANT& src, const BSTR defaultValue, BSTR* dest)
{
	CComBSTR tmp;

	switch (src.vt)
	{
	case VT_UI1:
		VarBstrFromUI1(src.bVal, NULL, NULL, &tmp);
		break;
	case VT_UI2:
		VarBstrFromUI2(src.uiVal, NULL, NULL, &tmp);
		break;
	case VT_UI4:
		VarBstrFromI4(src.ulVal, NULL, NULL, &tmp);
		break;
	case VT_UI8:
		VarBstrFromI8(src.ullVal, NULL, NULL, &tmp);
		break;
	case VT_I1:
		VarBstrFromI1(src.cVal, NULL, NULL, &tmp);
		break;
	case VT_I2:
		VarBstrFromI2(src.iVal, NULL, NULL, &tmp);
		break;
	case VT_I4:
		VarBstrFromI4(src.lVal, NULL, NULL, &tmp);
		break;
	case VT_I8:
		VarBstrFromI8(src.llVal, NULL, NULL, &tmp);
		break;
	case VT_R4:
		VarBstrFromR4(src.fltVal, NULL, NULL, &tmp);
		break;
	case VT_R8:
		VarBstrFromR8(src.dblVal, NULL, NULL, &tmp);
		break;
	case VT_BOOL:
		VarBstrFromBool(src.boolVal, NULL, NULL, &tmp);
		break;
	case VT_DECIMAL:
		VarBstrFromDec(src.pdecVal, NULL, NULL, &tmp);
		break;
	case VT_BSTR:
		tmp = src.bstrVal;
		break;
	default:
		tmp = *defaultValue;
	}
	
	*dest = tmp.Detach();

	return S_OK;
}

HRESULT utils::StringToVariant(const BSTR src, VARIANT* dest)
{
	HRESULT hr = S_OK;

	switch (dest->vt)
	{
	case VT_UI1:
		VarUI1FromStr(src, NULL, NULL, &dest->bVal);
		break;
	case VT_UI2:
		VarUI2FromStr(src, NULL, NULL, &dest->uiVal);
		break;
	case VT_UI4:
		VarUI4FromStr(src, NULL, NULL, &dest->ulVal);
		break;
	case VT_UI8:
		VarUI8FromStr(src, NULL, NULL, &dest->ullVal);
		break;
	case VT_I1:
		VarI1FromStr(src, NULL, NULL, &dest->cVal);
		break;
	case VT_I2:
		VarI2FromStr(src, NULL, NULL, &dest->iVal);
		break;
	case VT_I4:
		VarI4FromStr(src, NULL, NULL, &dest->lVal);
		break;
	case VT_I8:
		VarI8FromStr(src, NULL, NULL, &dest->llVal);
		break;
	case VT_R4:
		VarR4FromStr(src, NULL, NULL, &dest->fltVal);
		break;
	case VT_R8:
		VarR8FromStr(src, NULL, NULL, &dest->dblVal);
		break;
	case VT_BOOL:
		VarBoolFromStr(src, NULL, NULL, &dest->boolVal);
		break;
	case VT_DECIMAL:
		VarDecFromStr(src, NULL, NULL, dest->pdecVal);
		break;
	case VT_BSTR:
		dest->bstrVal = ::SysAllocString(src);
		break;
	default:
		hr = S_FALSE;
	}
	return hr;
}

template<>
int utils::Converter::Convert(const CString& attrString, int default)
{
	return default;
}

template<>
bool utils::Converter::Convert(const CString& attrString, bool default)
{
	CString tmp(attrString);
	tmp.MakeLower();
	if (tmp == _T("true"))
		return true;
	else if (tmp == _T("false"))
		return false;
	return default;
}