#pragma once

#include <Wininet.h>
#include <atlutil.h>
#include <strstream>
#include <GdiPlus.h>

typedef ATL::CAtlMap<ATL::CString,ATL::CString> CAtlStrMap;

namespace utils
{
	CString GetProgPath();
	CString GetFileExtension(const CString& str);

	CComQIPtr<IHTMLWindow2> IWebBrowserToIHTMLWindow(CComQIPtr<IWebBrowser2> spBrowser);

	CStringA GetFileDataA(const CString& path);
	CString GetFileData(const CString& path);

	HRESULT VariantToString(const VARIANT& src, const BSTR defaultValue, BSTR* dest);
	HRESULT StringToVariant(const BSTR src, VARIANT* dest);
	HRESULT ExposeObject(IWebBrowser2* pBrowser, LPCWSTR exposeName, IDispatch* pExpose);

	class Converter
	{
	public:
		template <typename T>
		static T Convert(const CString& attrString, T default);
	};
}


